#include "Follower.h"
Follower::Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries) { }
bool Follower::isNewerThanMe(int lastLogIndex, int lastLogTerm) const {
	if (!logEntries.size()) return true;
	if (logEntries.back().getTerm() == currentTerm) return logEntries.size() < lastLogIndex + 1;
	return currentTerm < logEntries.back().getTerm();
}
// 接收RequestVote
string Follower::requestVote(string requestVoteCodedIntoString) {
	RequestVote requestVote(requestVoteCodedIntoString);
	//直接返回false：term < currentTerm
	if (requestVote.getTerm() < currentTerm) return Answer(currentTerm, false).code();
	//如果 （votedFor == null || votedFor == candidateId） && candidate的log比当前节点新，投票给该节点，否则拒绝该节点
	if ((votedFor < 0 || votedFor == requestVote.getCandidateId())
		&& isNewerThanMe(requestVote.getLastLogIndex(), requestVote.getLastLogTerm())) {
		votedFor = requestVote.getCandidateId();
		return Answer(currentTerm, true).code();
	}
	return Answer(currentTerm, false).code();
}
// 接收AppendEntries
string Follower::appendEntries(string appendEntriesCodedIntoString) {
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// 超时计时器计数
	timeoutCounter.setReceiveInfoFlag();
	//直接返回false：term < currentTerm or prevLogIndex/Term对应的log不存在
	if ((appendEntries.getTerm() < currentTerm) 
		|| appendEntries.getPrevLogIndex() >= logEntries.size()
		|| logEntries[appendEntries.getPrevLogIndex()].getTerm() != appendEntries.getTerm())
		return Answer(currentTerm, false).code();
	int index = appendEntries.getPrevLogIndex() + 1;
	for (LogEntry entry : appendEntries.getEntries()) {
		//存在冲突entry，即相同的index，不同的term（就是leader没有的数据），删掉现有的entries并写入leader给的数据
		if (index < logEntries.size()) logEntries[index] = entry;
		//将不存在的entries追加到系统中
		else logEntries.push_back(entry);
		index++;
	}
	//当leaderCommit > commitIndex时，更新commitIndex = min(leaderCommit, 目前最新entry的index)
	if (appendEntries.getLeaderCommit() > commitIndex) {
		commitIndex = appendEntries.getLeaderCommit();
		if (commitIndex > logEntries.size() - 1) commitIndex = logEntries.size() - 1;
	}
	return Answer(currentTerm, true).code();
}
// 计算超时的线程
void Follower::timeoutCounterThread() {
	State::timeoutCounterThread();
	nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress,
		requestVoteAddress, commitIndex, lastApplied, logEntries);
}
// 跑起来，转化到下一个状态
State* Follower::run() {
	State::run();
	//waitThread();
	return nextState;
}