#include "Leader.h"
#include "Follower.h"
Leader::Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries) {
	// 开启接收start的线程
	startThread = new thread(&Leader::registerStart, this);
	// 上任的操作：发送心跳、初始化nextIndex和matchIndex
	for (auto follower = serverAddress.begin(); follower != serverAddress.end(); ++follower) {
		if (follower->first == ID) continue;
		// 初始化next为当前log的最后一个
		nextIndex[follower->first] = logEntries.size() - 1;
		// 初始化matchAddress为-1
		matchIndex[follower->first] = -1;
		// 发送心跳信息(非阻塞)
		sendAppendEntries(follower->first, nextIndex[follower->first], nextIndex[follower->first]);
		
	}
}
Leader::~Leader() {
	// 将start线程join一下
	startThread->join();
	// 释放线程对象
	delete startThread;
}

// 接收RequestVote
string Leader::requestVote(string requestVoteCodedIntoString) {
	receiveInfoLock.lock();
	
	RequestVote requestVote(requestVoteCodedIntoString);
	// term没有比当前leader大，可以直接拒绝，并返回当前的term
	if (requestVote.getTerm() <= currentTerm) {
		receiveInfoLock.unlock();
		return Answer(currentTerm, false).code();
	}
	// term更新，则退出当前状态，返回到Follower的状态
	currentTerm = requestVote.getTerm();
	
	// 理应在返回结果以后结束掉接收线程，但是此处无法这么处理
	// 所以用nextState作为信号量，保证线程间同步释放
	if (nextState && nextState->getCurrentTerm() < currentTerm) delete nextState;
	// 生成下一状态机
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		commitIndex, lastApplied, logEntries);
	receiveInfoLock.unlock();
	return Answer(currentTerm, true).code();
}
// 接收AppendEntries
string Leader::appendEntries(string appendEntriesCodedIntoString) {
	receiveInfoLock.lock();
	// 设置心跳信息
	timeoutCounter.setReceiveInfoFlag();
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// term没有比当前leader大，可以直接拒绝，并返回当前的term
	if (appendEntries.getTerm() <= currentTerm) {
		receiveInfoLock.unlock();
		return Answer(currentTerm, false).code();
	}
	// term更新，则退出当前状态，返回到Follower的状态
	currentTerm = appendEntries.getTerm();
	// 将entries添加到当前列表中（调用函数，还需要判断其能否添加，这一步其实已经算是follower的工作了）
	bool canAppend = appendEntriesReal(appendEntries.getPrevLogIndex(), appendEntries.getPrevLogTerm(),
		appendEntries.getLeaderCommit(), appendEntries.getEntries());
	// 生成下一状态机
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		commitIndex, lastApplied, logEntries);
	receiveInfoLock.unlock();
	return Answer(currentTerm, canAppend).code();
}
State* Leader::run() {
	// 开一条线程给start
	thread startThread(&Leader::registerStart, this);
	// 主线程处理发送appendEntries，该线程中检测其他线程是否有退出行为
	work();
	return nextState;
}

void Leader::start(AppendEntries newEntries) {
	receiveInfoLock.lock();
	//将client给的数据加入当前列表中
	merge(logEntries.begin(), logEntries.end(), newEntries.getEntries().begin(),
		newEntries.getEntries().end(), std::back_inserter(logEntries));
	// 有新增加的entries，更新lastApplied
	lastApplied += newEntries.getEntries().size();
	receiveInfoLock.unlock();
}
// 注册start函数
void Leader::registerStart() {
	startRpcServer.reset(nullptr);
	startRpcServer.reset(new rpc_server(startAddress.second, 6));
	startRpcServer->register_handler("start", [this](rpc_conn conn,
		string newEntries) {
			this->start(std::move(newEntries));
		});
	startRpcServer->run();//启动服务端
	cout << "Leader::registerStart close start" << endl;
}

//给其他所有进程同步log entries
void Leader::work() {
	// 用nextState作为同步信号量,超时/收到更新的信息的时候就可以退出了
	while (!nextState) {
		// 睡眠一段时间
		Sleep(300);
		// 循环遍历所有的follower，检测其nextIndex是否到最后
		// 1. 有返回值：
		//  1.0 返回值term更新，退为follower
		//	1.1 返回值为true：更新next和match，若next到头就发心跳(心跳的返回还需要再检查)
		//  1.2 返回值为false：next--，重发一次
		// 2. 无返回值：重发上一个包 
		for (auto follower = nextIndex.begin(); follower != nextIndex.end(); ++follower) {
			int followerID = follower->first;
			// 无返回值：重发上一个包
			if (!followerReturnVal[followerID]._Is_ready()) {
				resendAppendEntries(followerID);
				continue;
			}
			// 有返回值
			Answer answer(followerReturnVal[followerID].get());
			// 返回值term更新，退为follower
			if (answer.getTerm() > currentTerm) {
				nextState = new Follower(answer.getTerm(), ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries);
				timeoutCounter.stopCounter();
				return;
			}
			// 返回值为true：更新next和match，若next到头就发心跳
			if (answer.getSuccess()) {
				// 上一条不是心跳，需要更新next和match
				if (lastAppendEntries[followerID].getEntries().size()) {
					nextIndex[followerID] = lastAppendEntries[followerID].getPrevLogIndex()
						+ lastAppendEntries[followerID].getEntries().size() + 1;
					matchIndex[followerID] = lastAppendEntries[followerID].getPrevLogIndex()
						+ lastAppendEntries[followerID].getEntries().size();
				}
				// next到头了，需要发送心跳信息
				if (nextIndex[followerID] >= logEntries.size()) sendAppendEntries(followerID, -1, -1);
				// next没到头，将后续的都发过去
				else sendAppendEntries(followerID, nextIndex[followerID], logEntries.size() - 1);
			}
			// 返回值为false：next--，重发一次
			else {
				// needn't check the heartbreakt, cause if the heartbreak fail, it will be stop at the first one
				nextIndex[followerID]--;
				sendAppendEntries(followerID, nextIndex[followerID], nextIndex[followerID]);
			}
		}
	}
}
void Leader::sendAppendEntries(int followerID, int start, int end) {
	// 下标合法性判断
	if (end >= logEntries.size() || start > end) throw exception("Leader::sendAppendEntries: index is illegal");
	// follower id 合法性判断
	if (followerID < 0 || serverAddress.find(followerID) == serverAddress.end()) throw exception("Leader::sendAppendEntries: followerID is illegal");
	// 初始化appendEntries的内容
	int prevTerm = -1, prevIndex = -1;
	// 日志信息
	vector<LogEntry> entries;
	// 初始化日志信息
	if (start >= 0) 
		for (int index = start; index <= end; ++index) entries.push_back(logEntries[index]);
	// 待发送
	lastAppendEntries[followerID] = AppendEntries(currentTerm, ID, prevIndex, prevTerm, commitIndex, entries);
	// 异步调用 发送请求
	followerReturnVal[followerID] = 
		async(&RPC::invokeRemoteFunc, &rpc, serverAddress[followerID], "appendEntries", lastAppendEntries[followerID].code());
}
void Leader::resendAppendEntries(int followerID) {
	// 异步调用 发送请求
	followerReturnVal[followerID] =
		async(&RPC::invokeRemoteFunc, &rpc, serverAddress[followerID], "appendEntries", lastAppendEntries[followerID].code());
}
void Leader::stopThread() {
	State::stopThread();
	startRpcServer.reset(nullptr);
}