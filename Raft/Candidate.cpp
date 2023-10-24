#include "Candidate.h"
#include "Follower.h"
#include "Leader.h"
Candidate::Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress, commitIndex, lastApplied, logEntries), getVoteCounter(1)
	 {
	// 给自己投票
	votedFor = ID;
	// 初始化voteResult，并发送请求投票信息
	for (auto follower = serverAddress.begin(); follower != serverAddress.end(); ++follower) {
		int followerID = follower->first;
		if (followerID == ID) continue;
		voteResult[followerID] = 0;
		sendRequestVote(followerID);
	}
	// 开启计时器
	timeoutThread = new thread(&Candidate::timeoutCounterThread, this);
}
Candidate::~Candidate() {
	// join线程
	timeoutThread->join();
	// 释放对象
	delete timeoutThread;
}
// 接收RequestVote，不需要重置计时器，leader中计时器只运行一段
string Candidate::requestVote(string requestVoteCodedIntoString) {
	receiveInfoLock.lock();
	RequestVote requestVote(requestVoteCodedIntoString);
	// term没有比当前Candidate大，可以直接拒绝，并返回当前的term
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
		startAddress, commitIndex, lastApplied, logEntries);
	receiveInfoLock.unlock();
	return Answer(currentTerm, true).code();
}

void Candidate::timeoutCounterThread() {
	// 超时返回，转换到candidate
	if (timeoutCounter.run())
		nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
			startAddress, commitIndex, lastApplied, logEntries);
	// 未超时，主动返回，将nextState的初始化留给stop的调用处
}

// 接收AppendEntries，不需要重置计时器，leader中计时器只运行一段
// 只要对方的term不比自己小就接受对方为leader
string Candidate::appendEntries(string appendEntriesCodedIntoString) {
	receiveInfoLock.lock();
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// term没有比当前Candidate大，可以直接拒绝，并返回当前的term
	if (appendEntries.getTerm() < currentTerm) {
		receiveInfoLock.unlock();
		return Answer(currentTerm, false).code();
	}
	// term更新，则退出当前状态，返回到Follower的状态
	currentTerm = appendEntries.getTerm();
	// 将entries添加到当前列表中（调用函数，还需要判断其能否添加，这一步其实已经算是follower的工作了）
	bool canAppend = appendEntriesReal(appendEntries.getPrevLogIndex(), appendEntries.getPrevLogTerm(),
		appendEntries.getLeaderCommit(), appendEntries.getEntries());
	// 理应在返回结果以后结束掉接收线程，但是此处无法这么处理
	// 所以用nextState作为信号量，保证线程间同步释放
	if (!nextState || nextState->getCurrentTerm() <= currentTerm) {
		delete nextState;
		// 生成下一状态机
		nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
			startAddress, commitIndex, lastApplied, logEntries);
	}
	receiveInfoLock.unlock();
	return Answer(currentTerm, true).code();
}


// 发送投票信息
bool Candidate::checkRequestVote() {
	/*
	* 若目前都已经有返回值了，还没赢得选举，则退出当前状态，进入下一个candidate状态
	* 遍历所有还没投票的follower，除了自己
	* 1. 没有返回值：重发
	* 2. 有返回值：更新voteResult和getVoteCounter
	*/ 
	bool left = false;
	for (auto follower = voteResult.begin(); follower != voteResult.end(); ++follower) {
		// 如果已经返回过值了，则不需要再对其进行操作
		if (follower->second) continue;
		left = true;
		int followerID = follower->first;
		// 没有返回值：重发
		if (!followerReturnVal[followerID]._Is_ready()) sendRequestVote(followerID);
		// 有返回值：更新voteResult和getVoteCounter
		else {
			Answer answer(followerReturnVal[followerID].get());
			if (answer.getSuccess()) follower->second = 1, getVoteCounter++;
			else follower->second = -1;
		}
	}
	return left;
}
void Candidate::sendRequestVote(int followerID) {
	if (voteResult.find(followerID) == voteResult.end()) throw exception("Candidate::sendRequestVote follower doesn't exist.");
	RequestVote requestVoteContent(currentTerm, ID, logEntries.size() - 1, logEntries.size() ? logEntries.back().getTerm() : -1);
	followerReturnVal[followerID] =
		async(&RPC::invokeRemoteFunc, &rpc, serverAddress[followerID], "requestVote", requestVoteContent.code());
}
// 检测投票结果
bool Candidate::checkVoteResult() {
	if (getVoteCounter > voteResult.size() / 2) return true;
	return false;
}

void Candidate::work() {
	while (!nextState) {
		sleep_for(seconds(300));
		if (checkRequestVote()) {
			// 没有决出胜负，重开
			nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
				startAddress, commitIndex, lastApplied, logEntries);
			return;
		}
		// 选举成功
		if (checkVoteResult()) {
			nextState = new Leader(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
				startAddress, commitIndex, lastApplied, logEntries);
			return;
		}
	}
}

// 运行该机器，返回值是下一个状态
State* Candidate::run() {
	State::run();
	// 停止当前计数器
	timeoutCounter.stopCounter();
	return nextState;
}