#include "Candidate.h"
Candidate::Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, ServerState::Candidate, commitIndex, lastApplied, logEntries), 
	getVote(0){
	voteResult = vector<int>(serverAddress.size(), 0);
}
// 接收RequestVote
string Candidate::requestVote(string requestVoteCodedIntoString) {
	RequestVote requestVote(requestVoteCodedIntoString);
	// term没有比当前Candidate大，可以直接拒绝，并返回当前的term
	if (requestVote.getTerm() <= currentTerm) return to_string(currentTerm) + " 0";
	// term更新，则退出当前状态，返回到Follower的状态
	// 停止当前节点运行，并转向follower，这里需要多开一个线程，且该线程需要等待一段时间&&detach
	return to_string(currentTerm) + " 1";
}
// 接收AppendEntries
string Candidate::appendEntries(string appendEntriesCodedIntoString) {
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// term没有比当前Candidate大，可以直接拒绝，并返回当前的term
	if (appendEntries.getTerm() <= currentTerm) return to_string(currentTerm) + " 0";
	// term更新，则退出当前状态，返回到Follower的状态
	// 停止当前节点运行，并转向follower，这里需要多开一个线程，且该线程需要等待一段时间&&detach
	return to_string(currentTerm) + " 1";
}