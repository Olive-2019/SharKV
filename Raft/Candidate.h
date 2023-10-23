#pragma once
#include "State.h"

class Candidate : public State
{
	// 投票结果 0:没有收到 1：赢得该选票 -1：输了
	map<int, int> voteResult;
	// 获得的选票数量
	int getVoteCounter;
	
	// 运行检测投票
	void work();
	// 检查发送的投票信息，返回值为true：都返回了 false：存在未返回的值
	bool checkRequestVote();
	// 根据当前信息，发送requestVote
	void sendRequestVote(int followerID);
	
	// 检测投票结果，只有返回true时知道选举成功，返回false意味着未知数，选举失败靠appendEntries
	bool checkVoteResult();

	// 用于异步接收心跳/返回值的future
	map<int, shared_future<string>> followerReturnVal;
public:
	Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// 接收RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// 接收AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
	// 运行该机器，返回值是下一个状态
	State* run();

};

