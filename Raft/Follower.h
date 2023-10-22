#pragma once
#include "State.h"
#include "Candidate.h"
class Follower : public State
{
	// 比较传入记录和自己哪个更新
	bool isNewerThanMe(int lastLogIndex, int lastLogTerm) const;
	// 下一个状态
	State* nextState;
	// 计算超时的线程
	void timeoutCounterThread();
public:
	Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
		NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// 接收RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// 接收AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
	// 运行该机器，返回值是下一个状态
	State* run();
};

