#pragma once
#include "State.h"
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
};

