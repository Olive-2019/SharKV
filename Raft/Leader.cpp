#include "Leader.h"
#include "Follower.h"
Leader::Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor, int maxResendNum) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress, commitIndex, lastApplied, logEntries, votedFor),
    maxResendNum(maxResendNum){
	
}
Leader::~Leader() {
	if (debug) cout << ID << " will not be Leader any more." << endl;
}

// 接收RequestVote
string Leader::requestVote(string requestVoteCodedIntoString) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	RequestVote requestVote(requestVoteCodedIntoString);
	if (debug) cout << ID << " receive requestVote Msg from " << requestVote.getCandidateId() << endl;
	// term没有比当前leader大，可以直接拒绝，并返回当前的term
	if (requestVote.getTerm() <= currentTerm) {
		if (debug) cout << "reject " << requestVote.getCandidateId() << ", cause its term is old." << endl;
		return Answer(currentTerm, false).code();
	}
	// term更新，则退出当前状态，返回到Follower的状态
	currentTerm = requestVote.getTerm();
	
	// 理应在返回结果以后结束掉接收线程，但是此处无法这么处理
	// 所以用nextState作为信号量，保证线程间同步释放
	if (nextState && nextState->getCurrentTerm() < currentTerm) delete nextState;
	// 生成下一状态机
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		startAddress, commitIndex, lastApplied, logEntries, votedFor = requestVote.getCandidateId());
	if (debug) cout << "vote for " << requestVote.getCandidateId() << "." << endl;
	return Answer(currentTerm, true).code();
}
// 接收AppendEntries
string Leader::appendEntries(string appendEntriesCodedIntoString) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (debug) cout << ID << " receive appendEntries Msg" << endl;
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// timeoutCounter.setReceiveInfoFlag();
	// term没有比当前leader大，可以直接拒绝，并返回当前的term
	if (appendEntries.getTerm() <= currentTerm) {
		if (debug) cout << "reject " << appendEntries.getLeaderId() << "'s appendEntries, cause its term is old." << endl;
		return Answer(currentTerm, false).code();
	}
	// term更新，则退出当前状态，返回到Follower的状态
	currentTerm = appendEntries.getTerm();
	// 将entries添加到当前列表中（调用函数，还需要判断其能否添加，这一步其实已经算是follower的工作了）
	bool canAppend = appendEntriesReal(appendEntries.getPrevLogIndex(), appendEntries.getPrevLogTerm(),
		appendEntries.getLeaderCommit(), appendEntries.getEntries());
	if (!nextState || nextState->getCurrentTerm() <= currentTerm) {
		delete nextState;
		// 生成下一状态机
		nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
			startAddress, commitIndex, lastApplied, logEntries);
	} 
	return Answer(currentTerm, canAppend).code();
}

void Leader::checkFollowers() {
	// 循环遍历所有的follower，检测其nextIndex是否到最后
		// 1. 有返回值：
		//  1.0 返回值term更新，退为follower
		//	1.1 返回值为true：更新next和match，若next到头就发心跳(心跳的返回还需要再检查)
		//  1.2 返回值为false：next--，重发一次
		// 2. 无返回值：重发上一个包 
	for (auto follower = nextIndex.begin(); follower != nextIndex.end(); ++follower) {
		int followerID = follower->first;
		// 无返回值：重发上一个包
		if (!checkOneFollowerReturnValue(followerID)) continue;
		// 有返回值
		Answer answer = getOneFollowerReturnValue(followerID);
		if (debug) cout << "receive the return value of " << followerID << ", and its result is " << answer.getSuccess() << endl;
		// 返回值term更新，退为follower
		if (answer.getTerm() > currentTerm) {
			nextState = new Follower(answer.getTerm(), ID, appendEntriesAddress, requestVoteAddress,
				startAddress, commitIndex, lastApplied, logEntries);
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
// 检查单个follower，若成功则true，若不成功则尝试重发
bool Leader::checkOneFollowerReturnValue(int followerID) {
	// 遍历follower的返回值
	for (int i = 0; i < followerReturnVal[followerID].size(); ++i) 
		if (followerReturnVal[followerID][i]._Is_ready()) return true;
	// 都没有返回，则尝试重发
	sendAppendEntries(followerID);
	return false;
}
// 获取单个follower的返回值
Answer Leader::getOneFollowerReturnValue(int followerID) {
	for (int i = 0; i < followerReturnVal[followerID].size(); ++i)
		if (followerReturnVal[followerID][i]._Is_ready()) {
			Answer ans(followerReturnVal[followerID][i].get());
			followerReturnVal[followerID].clear();
			return ans;
		}
	throw exception("Leader::getOneFollowerReturnValue Logical Error: didn't hava return value.");
	return Answer(0, false);
}

void Leader::updateCommit() {
	// commit超过半数follower可以match的log entries
	while (commitIndex < logEntries.size()) {
		int counter = 0;
		for (auto it = matchIndex.begin(); it != matchIndex.end(); ++it)
			if (commitIndex + 1 >= it->second) counter++;
		if (counter > matchIndex.size() / 2) commitIndex++;
		else break;
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
	if (start >= 0) {
		prevIndex = start - 1;
		// 若前一个存在，即现在发的不是第一条
		if (prevIndex >= 0) prevTerm = logEntries[prevIndex].getTerm();
		for (int index = start; index <= end; ++index) entries.push_back(logEntries[index]);
	}
		
	// 待发送
	lastAppendEntries[followerID] = AppendEntries(currentTerm, ID, prevIndex, prevTerm, commitIndex, entries);
	// 异步调用 发送请求
	sendAppendEntries(followerID);
	
}
bool Leader::sendAppendEntries(int followerID) {
	if (serverAddress.find(followerID) == serverAddress.end()) throw exception("Leader::resendAppendEntries follower doesn't exist.");
	// 如果已经超出最大重发次数，则不重发，直接返回
	if (followerReturnVal[followerID].size() >= maxResendNum) return false;
	// 异步调用 发送请求
	followerReturnVal[followerID].push_back(
		async(&RPC::invokeRemoteFunc, &rpc, serverAddress[followerID], 
			"appendEntries", lastAppendEntries[followerID].code()));
	if (debug) cout << "send appendEntries to " << followerID << endl;
	return true;
}

//给其他所有进程同步log entries
void Leader::work() {


	if (debug) cout << endl << ID << " become Leader" << endl;

	// 读入集群中所有server的地址，leader读入AppendEntriesAddress的地址
	ServerAddressReader serverAddressReader("AppendEntriesAddress.conf");

	serverAddress = serverAddressReader.getNetWorkAddresses();
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

	// 用nextState作为同步信号量,超时/收到更新的信息的时候就可以退出了
	while (!nextState) {
		// 睡眠一段时间
		sleep_for(seconds(15));
		checkFollowers();
		updateCommit();
	}
}
