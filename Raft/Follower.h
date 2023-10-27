#pragma once
#include "State.h"
#include "Candidate.h"
class Follower : public State
{
	// 用于计算超时的类
	TimeoutCounter timeoutCounter;
	// 计时器线程
	thread* timeoutThread;
	// 计算超时的线程，结束时会把其他几个接收线程都退出，所以如果想要结束当前所有线程，可以调用结束计时器的函数
	void timeoutCounterThread();

	// 记录leaderID
	int leaderID;
	// 比较传入记录和自己哪个更新
	bool isNewerThanMe(int lastLogIndex, int lastLogTerm) const;
	// 重载统一接口，在follower里面就是一个守护线程
	void work();
public:
	Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries,
		int votedFor = -1);
	~Follower();
	// 重载start调用,转发给leader
	void start(AppendEntries newEntries);
	// 接收RequestVote
	Answer requestVote(string requestVoteCodedIntoString);
	// 接收AppendEntries
	Answer appendEntries(string appendEntriesCodedIntoString);
};

