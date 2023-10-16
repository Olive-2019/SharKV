#pragma once
#include "State.h"
#include "Candidate.h"
class Follower : public State
{
	bool isNewerThanMe(int lastLogIndex, int lastLogTerm) const;
public:
	Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
		NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// 接收RequestVote
	string requestVote(rpc_conn conn, string requestVoteCodedIntoString);
	// 接收AppendEntries
	string appendEntries(rpc_conn conn, string appendEntriesCodedIntoString);
	// 运行该机器，返回值是下一个状态
	State* run();
};

