#pragma once
#include "ServerState.h"
#include "LogEntry.h"
#include "TimeoutCounter.h"
#include <map>
using std::pair;
class State
{
protected:
	typedef pair<string, int> psi;
	/*需要持久化的state*/
	//server所处的term，也是该server所得知的最新term
	int currentTerm;
	//如果是-1，则该term内尚未投票，如果是candidateID，则该term中的选举已经投票给了该candidate
	int votedFor;
	// 当前server的所有log entries，每一条log entry包含命令和term编号
	vector<LogEntry> logEntries;
	//状态机id
	int ID;
	// 状态机用于接收appendEntries的ip和port
	psi appendEntriesAddress;
	// 状态机用于接收requestVote的ip和port
	psi requestVoteAddress;

	/*易失状态，不需要持久化*/
	// 当前状态
	ServerState state;
	//最大的已经commit的log entries index
	int commitIndex;
	//最新加入的log entries index
	int lastApplied;
	// 用于计算超时的类
	TimeoutCounter timeoutCounter;


	// 计算超时的线程
	void timeoutCounterThread();
	// 等待接收AppendEntries
	void receiveAppendEntries();
	// 注册等待接收AppendEntries
	void registerReceiveAppendEntries();
	// 注册投票线程RequestVote
	void registerReceiveRequestVote();
	// 投票线程RequestVote
	void receiveRequestVote();
public:
	// 运行该机器，返回值是下一个状态
	virtual State* run();
};

