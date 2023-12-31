#include "Leader.h"
#include "Follower.h"
Leader::Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, 
	vector<LogEntry> logEntries, int votedFor, int maxResendNum, int snapshotThreshold) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress, applyMessageAddress,
		commitIndex, lastApplied, logEntries, votedFor), maxResendNum(maxResendNum), snapshotThreshold(snapshotThreshold) {
	if (debug) cout << "Leader::Leader new a leader" << endl;
	// 读入集群中所有server的地址，leader读入AppendEntriesAddress的地址
	ServerAddressReader serverAddressReader("AppendEntriesAddress.conf");

	serverAddress = serverAddressReader.getNetWorkAddresses();
	// 上任的操作：发送心跳、初始化nextIndex和matchIndex
	for (auto follower = serverAddress.begin(); follower != serverAddress.end(); ++follower) {
		int followerID = follower->first;
		// 不初始化自己
		if (followerID == ID) continue;
		// 初始化next为当前log的最后一个
		nextIndex[followerID] = logEntries.size() - 1;
		// 初始化matchAddress为-1
		matchIndex[followerID] = -1;
		// 发送心跳信息(非阻塞)
		sendAppendEntries(followerID, nextIndex[followerID], nextIndex[followerID]);
	}
}
Leader::~Leader() {
	if (debug) cout << ID << " will not be Leader any more." << endl;
}

// 接收RequestVote
Answer Leader::requestVote(rpc_conn conn, RequestVote requestVote) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (nextState) return nextState->requestVote(conn, requestVote);
	//RequestVote requestVote(requestVoteCodedIntoString);
	if (debug) cout << "Leader::requestVote " << ID << " receive requestVote Msg from " << requestVote.getCandidateId() << endl;
	//if (debug) cout << ID << " receive requestVote Msg from " << requestVote.getCandidateId() << endl;
	// term没有比当前leader大，可以直接拒绝，并返回当前的term
	if (requestVote.getTerm() <= currentTerm) {
		if (debug) cout << "reject " << requestVote.getCandidateId() << ", cause its term is old." << endl;
		return Answer( currentTerm, false );
	}
	// term更新，则退出当前状态，返回到Follower的状态
	currentTerm = requestVote.getTerm();
	
	// 理应在返回结果以后结束掉接收线程，但是此处无法这么处理
	// 所以用nextState作为信号量，保证线程间同步释放
	if (nextState && nextState->getCurrentTerm() < currentTerm) delete nextState;
	// 生成下一状态机
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries, votedFor = requestVote.getCandidateId());
	if (debug) cout << "vote for " << requestVote.getCandidateId() << "." << endl;
	return Answer( currentTerm, true );
}
// 接收AppendEntries
Answer Leader::appendEntries(rpc_conn conn, AppendEntries appendEntries) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (nextState) return nextState->appendEntries(conn, appendEntries);
	//AppendEntries appendEntries(appendEntriesCodedIntoString);
	if (debug) cout << "Leader::appendEntries " << ID << " receive appendEntries Msg from " << appendEntries.getLeaderId() << endl;
	// timeoutCounter.setReceiveInfoFlag();
	// term没有比当前leader大，可以直接拒绝，并返回当前的term
	if (appendEntries.getTerm() < currentTerm) {
		if (debug) cout << "Leader::appendEntries " << "reject " << appendEntries.getLeaderId() << "'s appendEntries, cause its term is old." << endl;
		return Answer( currentTerm, false);
	}
	// term更新，则退出当前状态，返回到Follower的状态
	currentTerm = appendEntries.getTerm();
	// 生成下一状态机
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries);// 将entries添加到当前列表中（调用函数，还需要判断其能否添加，这一步其实已经算是follower的工作了）
	return nextState->appendEntries(conn, appendEntries);
	//bool canAppend = appendEntriesReal(appendEntries.getPrevLogIndex(), appendEntries.getPrevLogTerm(),
		//appendEntries.getLeaderCommit(), appendEntries.getEntries());
	
	//return Answer(currentTerm, canAppend);
}

void Leader::checkFollowers() {
	// 循环遍历所有的follower，检测其nextIndex是否到最后
		// 1. 有返回值：
		//  1.0 返回值term更新，退为follower
		//	1.1 返回值为true：更新next和match，若next到头就发心跳(心跳的返回还需要再检查)
		//  1.2 返回值为false：next--，重发一次
		// 2. 无返回值：重发上一个包 
	for (auto follower = nextIndex.begin(); follower != nextIndex.end(); ++follower) {
		lock_guard<mutex> lockGuard(receiveInfoLock);
		int followerID = follower->first;
		// 无返回值：重发上一个包
		if (!checkOneFollowerReturnValue(followerID)) continue;
		// 有返回值
		const Answer answer = getOneFollowerReturnValue(followerID);
		//if (debug) cout << "appendEntries get value from " << followerID << " content: term " << answer.term << " success " << answer.success << endl;
		// 返回值term更新，退为follower
		if (answer.getTerm() > currentTerm) {
			nextState = new Follower(answer.getTerm(), ID, appendEntriesAddress, requestVoteAddress,
				startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries);
			return;
		}
		// 返回值为true：更新next和match，若next到头就发心跳
		if (answer.isSuccess()) {
			// 上一条不是心跳，需要更新next和match
			if (lastAppendEntries[followerID].getEntries().size()) {
				nextIndex[followerID] = lastAppendEntries[followerID].getPrevLogIndex()
					+ lastAppendEntries[followerID].getEntries().size() + 1;
				matchIndex[followerID] = lastAppendEntries[followerID].getPrevLogIndex()
					+ lastAppendEntries[followerID].getEntries().size();
			}
			int logEntriesNum = logEntries.size();
			// next到头了，需要发送心跳信息(这个地方的问题，执行逻辑貌似有问题，初步猜测是没有加锁导致的)
			if (nextIndex[followerID] >= logEntriesNum || !logEntriesNum) {
				if (debug) cout << "Leader::checkFollowers nextIndex " << nextIndex[followerID] << " logEntries.size " << logEntriesNum << endl;
				sendAppendEntries(followerID, -1, -1);
			}
			else {
				// next没到头，将后续的都发过去
				//if (nextIndex[followerID] < 0) nextIndex[followerID] = logEntriesNum - 1;
				sendAppendEntries(followerID, nextIndex[followerID], logEntriesNum - 1);
			}
			
		}
		// 返回值为false：next--，重发一次
		else {
			if (nextIndex[followerID] < 0) continue;
			// needn't check the heartbreakt, cause if the heartbreak fail, it will be stop at the first one
			nextIndex[followerID]--;
			sendAppendEntries(followerID, nextIndex[followerID], logEntries.size() - 1);
		}
	}
}
// 检查单个follower，若成功则true，若不成功则尝试重发
bool Leader::checkOneFollowerReturnValue(int followerID, bool snapshot) {
	// 遍历follower的返回值，需要区分是否snapshot的
	vector<shared_future<Answer>>& checkReturnVal = snapshot ? snapshotReturnVal[followerID] : followerReturnVal[followerID];
	for (auto val = checkReturnVal.begin(); val != checkReturnVal.end(); ) {
		// 获取调用状态
		future_status status = val->wait_for(seconds(0));
		// 只要有一个已经成功返回就会返回true
		if (status == future_status::ready) return true;
		else if (status == future_status::timeout) {
			// 超时删除
			if (snapshot) val = snapshotReturnVal[followerID].erase(val);
			else val = followerReturnVal[followerID].erase(val);
		}
		else val++;
	}
	// 都没有返回，则尝试重发
	sendAppendEntries(followerID, snapshot);
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
	return Answer( 0, false );
}

void Leader::updateCommit() {
	// commit超过半数follower可以match的log entries
	while (commitIndex < logEntries.size()) {
		int counter = 0;
		for (auto it = matchIndex.begin(); it != matchIndex.end(); ++it)
			if (commitIndex + 1 >= it->second) counter++;
			//else break;
		if (counter + 1 > matchIndex.size() / 2) commitIndex++;
		else break;
	}
	if (commitIndex >= logEntries.size()) commitIndex = logEntries.size() - 1;
	//if (debug) cout << "Leader::updateCommit commitIndex " << commitIndex << endl;
	// 若未超过阈值，则正常applyMsg
	if (commitIndex >= 0 && commitIndex < snapshotThreshold) applyMsg();
	// 若commit的数量超过阈值，则要开一条线程执行快照操作
	else if (commitIndex >= snapshotThreshold) async(&Leader::snapshot, this);
}



void Leader::sendAppendEntries(int followerID, int start, int end, bool snapshot, int snapshotIndex) {
	// 下标合法性判断
	if (start >= 0 && (end >= logEntries.size() || start > end)) {
		if (debug) cout << "Leader::sendAppendEntries: start " << start << " end " << end << " logEntries.size() " 
			<< logEntries.size() << " snapshot " << snapshot << " snapshotIndex " << snapshotIndex << endl;
		throw exception("Leader::sendAppendEntries: index is illegal");
	}
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
	//if (debug) cout << "Leader::sendAppendEntries snapshotIndex " << snapshotIndex << endl;
	// 待发送
	if (snapshot) snapshotLastAppendEntries[followerID] = AppendEntries(currentTerm, ID, prevIndex, prevTerm, snapshotIndex, entries, snapshot);
	else lastAppendEntries[followerID] = AppendEntries(currentTerm, ID, prevIndex, prevTerm, commitIndex, entries, snapshot);
	// 异步调用 发送请求
	sendAppendEntries(followerID, snapshot);
	
}
bool Leader::sendAppendEntries(int followerID, bool snapshot) {
	if (serverAddress.find(followerID) == serverAddress.end()) throw exception("Leader::resendAppendEntries follower doesn't exist.");
	int sendNum = followerReturnVal[followerID].size();
	AppendEntries sendEntries = lastAppendEntries[followerID];
	if (snapshot) sendNum = snapshotReturnVal[followerID].size(), sendEntries = snapshotLastAppendEntries[followerID];
	// 如果已经超出最大重发次数，则不重发，直接返回
	if (sendNum >= maxResendNum) return false;
	// 异步调用 发送请求

	if (debug) cout << "Leader::sendAppendEntries send appendEntries to " << followerID << " content size is " << sendEntries.getEntries().size() << " snapshot " << snapshot << " commitIndex " << sendEntries.getLeaderCommit() << endl;
	shared_future<Answer> ans = async(&RPC::invokeAppendEntries, &rpc, serverAddress[followerID], sendEntries);
	// 将异步结果放入队列中
	if (snapshot) snapshotReturnVal[followerID].push_back(ans);
	else followerReturnVal[followerID].push_back(ans);
	//if (debug) cout << "Leader::sendAppendEntries send appendEntries to " << followerID << " content size is " << sendEntries.getEntries().size() << endl;
	return true;
}

//给其他所有进程同步log entries
void Leader::work() {

	if (debug) cout << endl << ID << " work as Leader" << endl;
	if (nextState) return;
	

	// 用nextState作为同步信号量,超时/收到更新的信息的时候就可以退出了
	while (!nextState) {
		// 睡眠一段时间
		sleep_for(seconds(2));
		checkFollowers();
		updateCommit();
		persistence();
		//if (debug) cout << "Leader::work one round" << endl;
		// 模拟停机
		//if (crush(0.7)) break;
	}
}

// 注册等待接收AppendEntries句柄
void Leader::registerHandleAppendEntries() {
	appendEntriesRpcServer->register_handler("appendEntries", &Leader::appendEntries, this);
}
// 注册投票线程RequestVote句柄
void Leader::registerHandleRequestVote() {
	requestVoteRpcServer->register_handler("requestVote", &Leader::requestVote, this);
}
// 注册start函数句柄
void Leader::registerHandleStart() {
	startRpcServer->register_handler("start", &Leader::start, this);
}

void Leader::snapshot() {
	int snapshotIndex = commitIndex;
	if (debug) cout << "Leader::snapshot  snapshotIndex " << snapshotIndex << endl;
	// 通知每一个系统存快照，若半数以上通过则可以进入下一步（会阻塞一段时间）
	informSnapshot(snapshotIndex);
	// 修改自己的状态，通知上层应用写快照
	snapShotModifyState(snapshotIndex);
}
void Leader::informSnapshot(int snapshotIndex) {
	map<int, bool> hasReturn;
	int counter = 0;
	for (auto follower = followerReturnVal.begin(); follower != followerReturnVal.end(); ++follower) {
		int followerID = follower->first;
		// 发送snapshot的AppendEntries
		sendAppendEntries(followerID, 0, snapshotIndex, true, snapshotIndex);
		// 初始化返回值
		hasReturn[followerID] = false;
	}
	// 检测是否过半，没过半就不停重发
	while (counter < serverAddress.size() / 2) {
		for (auto follower = followerReturnVal.begin(); follower != followerReturnVal.end(); ++follower) {
			int followerID = follower->first;
			if (hasReturn[followerID]) continue;
			if (checkOneFollowerReturnValue(followerID, true)) {
				hasReturn[followerID] = true;
				//nextIndex[followerID] -= (snapshotIndex + 1);
				counter++;
			}
		}
	}
}


void Leader::snapShotModifyState(int snapshotIndex) {
	// 需要修改状态，所以不能让其他接受线程改状态
	lock_guard<mutex> lockGuard(receiveInfoLock);
	State::snapShotModifyState(snapshotIndex);
	for (auto follower = nextIndex.begin(); follower != nextIndex.end(); ++follower) {
		int followerID = follower->first;
		// 重置next
		nextIndex[followerID] -= (snapshotIndex + 1);
		if (nextIndex[followerID] < -1) nextIndex[followerID] = -1;
		// 重置matchAddress
		matchIndex[followerID] -= (snapshotIndex + 1);
		if (matchIndex[followerID] < 0) matchIndex[followerID] = 0;
	}
}
