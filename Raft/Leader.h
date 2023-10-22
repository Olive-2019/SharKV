#pragma once
#include "State.h"
class Leader : public State
{
	// 需要发给每一个follower的下一条log entry（初始化值是leader的最后一个log entry的下一个值）
	vector<int> nextIndex;
	// 每个follower当前匹配到哪一条log entry（初始化值为0）
	vector<int> matchIndex;

	
public:
	Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
		NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// 接收RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// 接收AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
	// 运行该机器，返回值是下一个状态
	State* run();
};

