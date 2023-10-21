#include "Candidate.h"
Candidate::Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries), 
	getVote(0){
	voteResult = vector<int>(serverAddress.size(), 0);
}
// 接收RequestVote
string Candidate::requestVote(rpc_conn conn, string requestVoteCodedIntoString) {
	RequestVote requestVote(requestVoteCodedIntoString);
	// term没有比当前Candidate大，可以直接拒绝，并返回当前的term
	if (requestVote.getTerm() <= currentTerm) return Answer(currentTerm, false).code();
	// term更新，则退出当前状态，返回到Follower的状态
	// 停止当前节点运行，并转向follower，这里需要多开一个线程，且该线程需要等待一段时间&&detach
	return Answer(currentTerm, true).code();
}
// 接收AppendEntries
string Candidate::appendEntries(rpc_conn conn, string appendEntriesCodedIntoString) {
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// term没有比当前Candidate大，可以直接拒绝，并返回当前的term
	if (appendEntries.getTerm() <= currentTerm) return Answer(currentTerm, false).code();
	// term更新，则退出当前状态，返回到Follower的状态
	// 停止当前节点运行，并转向follower，这里需要多开一个线程，且该线程需要等待一段时间&&detach
	return Answer(currentTerm, true).code();
}
// 运行该机器，返回值是下一个状态
State* Candidate::run() {

}