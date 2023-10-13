#pragma once
#include "source.h"
#include "LogEntry.h"
#include "TimeoutCounter.h"
#include "ServerAddressReader.h"
#include "RequestVote.h"
#include "AppendEntries.h"
#include <map>
using std::pair;
using std::map;

class State
{
protected:
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
	NetWorkAddress appendEntriesAddress;
	// 状态机用于接收requestVote的ip和port
	NetWorkAddress requestVoteAddress;
	// 集群中各服务器的地址(leader用于发送AppendEntries，candidate用于发送requestVote，follower用于转发请求)
	map<int, NetWorkAddress> serverAddress;


	/*易失状态，不需要持久化*/
	// 当前状态
	ServerState state;
	//最大的已经commit的log entries index
	int commitIndex;
	//最新加入的log entries index
	int lastApplied;
	
	/*运行过程中需要用到的变量*/
	// 用于计算超时的类
	TimeoutCounter timeoutCounter;




	// 计算超时的线程
	void timeoutCounterThread();
	// 等待接收AppendEntries
	virtual string appendEntries(string appendEntriesCodedIntoString) = 0;
	// 注册等待接收AppendEntries
	void registerAppendEntries();
	// 投票线程RequestVote
	virtual string requestVote(string requestVoteCodedIntoString) = 0;
	// 注册投票线程RequestVote
	void registerRequestVote();
public:
	State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,NetWorkAddress requestVoteAddress, 
		ServerState state, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// 运行该机器，返回值是下一个状态
	virtual State* run() = 0;
};

