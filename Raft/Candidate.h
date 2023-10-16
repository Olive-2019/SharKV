#pragma once
#include "State.h"
class Candidate : public State
{
	// 投票结果 0:没有收到 1：赢得该选票 -1：输了
	vector<int> voteResult;
	int getVote;
public:
	Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
		NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// 接收RequestVote
	string requestVote(rpc_conn conn, string requestVoteCodedIntoString);
	// 接收AppendEntries
	string appendEntries(rpc_conn conn, string appendEntriesCodedIntoString);
};

