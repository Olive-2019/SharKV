#include "Candidate.h"
#include "Follower.h"
#include "Leader.h"
Candidate::Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries), 
	getVote(0){
	for (auto follower = serverAddress.begin(); follower != serverAddress.end(); ++follower) {
		int followerID = follower->first;
		voteResult[followerID] = 0;
	}
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
		commitIndex, lastApplied, logEntries);
	receiveInfoLock.unlock();
	return Answer(currentTerm, true).code();
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
			commitIndex, lastApplied, logEntries);
	}
	receiveInfoLock.unlock();
	return Answer(currentTerm, true).code();
}
// 运行该机器，返回值是下一个状态
State* Candidate::run() {
	work();
	return nextState;
}

void Candidate::work() {
	while (!nextState) {
		sendRequestVote();
		// 选举成功
		if (checkVoteResult()) {
			nextState = new Leader(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries);
			timeoutCounter.stopCounter();
			return;
		}
	}
}
// 发送投票信息
void Candidate::checkRequestVote() {
	// 装配requestVote内容
	int lastIndex = -1, lastTerm = -1;
	if (logEntries.size()) lastTerm = logEntries.back().getTerm();
	RequestVote requestVote(currentTerm, ID, lastIndex, lastTerm);
	/*
	* 若目前都已经有返回值了，还没赢得选举，则退出当前状态，进入下一个candidate状态
	* 遍历所有还没投票的follower，除了自己
	* 1. 没有返回值：重发
	* 2. 有返回值：更新voteRes和getVoteCounter
	*	
	*/ 

	for (auto follower = voteResult.begin(); follower != voteResult.end(); ++follower) {
		if (follower->second) continue;
		int followerID = follower->first;
		
	}
}

// 检测投票结果
bool Candidate::checkVoteResult() {
	if (getVoteCounter > voteResult.size() / 2) return true;
	return false;
}