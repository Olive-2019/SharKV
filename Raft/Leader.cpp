#include "Leader.h"
Leader::Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries) {
	
	// 发送心跳
}
// 接收RequestVote
string Leader::requestVote(rpc_conn conn, string requestVoteCodedIntoString) {
	RequestVote requestVote(requestVoteCodedIntoString);
	// term没有比当前leader大，可以直接拒绝，并返回当前的term
	if (requestVote.getTerm() <= currentTerm) return to_string(currentTerm) + " 0";
	// term更新，则退出当前状态，返回到Follower的状态
	// 停止当前节点运行，并转向follower，这里需要多开一个线程，且该线程需要等待一段时间&&detach
	return to_string(currentTerm) + " 1";
}
// 接收AppendEntries
string Leader::appendEntries(rpc_conn conn, string appendEntriesCodedIntoString) {
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// term没有比当前leader大，可以直接拒绝，并返回当前的term
	if (appendEntries.getTerm() <= currentTerm) return to_string(currentTerm) + " 0";
	// term更新，则退出当前状态，返回到Follower的状态
	// 停止当前节点运行，并转向follower，这里需要多开一个线程，且该线程需要等待一段时间&&detach
	return to_string(currentTerm) + " 1";
}