#pragma once
#include "source.h"
#include "LogEntry.h"
#include "TimeoutCounter.h"
#include "ServerAddressReader.h"
#include "RequestVote.h"
#include "AppendEntries.h"
#include "Answer.h"
#include <thread>
#include "../rest_rpc/include/rest_rpc.hpp"
using namespace rest_rpc::rpc_service;
using std::unique_ptr;
using namespace rest_rpc;
using std::thread;
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
	//最大的已经commit的log entries index
	int commitIndex;
	//最新加入的log entries index
	int lastApplied;
	
	/*运行过程中需要用到的变量*/
	// 用于计算超时的类
	TimeoutCounter timeoutCounter;
	// 计时器线程
	thread* timeoutThread;
	// AppendEntries线程
	thread* appendEntriesThread;
	// RequestVote线程
	thread* requestVoteThread;

	/*控制线程的智能指针*/
	// 用于控制接收AppendEntries线程
	unique_ptr<rpc_server> appendEntriesRpcServer;
	// 用于控制接收RequestVote线程
	unique_ptr<rpc_server> requestVoteRpcServer;

	// 下一个状态，非空说明要进入下一个状态了，马上跳出循环，stop
	State* nextState;
	// 接收信息的大锁，进程中同时仅允许处理一条接收的信息，包括appendEntries和requestVote
	mutex receiveInfoLock;

	// 计算超时的线程
	virtual void timeoutCounterThread();
	// 注册等待接收AppendEntries
	void registerAppendEntries();
	// 注册投票线程RequestVote
	void registerRequestVote();

	// 停止接收投票和心跳线程
	virtual void stopThread();
	// 等待接收投票和心跳线程join
	virtual void waitThread();
	// 添加entries，返回值表示是否成功添加
	bool appendEntriesReal(int prevLogIndex, int prevLogTerm, int leaderCommit, vector<LogEntry> entries);
public:
	State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,NetWorkAddress requestVoteAddress, 
	 int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// 获取当前currentTerm
	int getCurrentTerm() const;
	// 等待接收AppendEntries
	virtual string appendEntries(string appendEntriesCodedIntoString) = 0;
	// 投票线程RequestVote
	virtual string requestVote(string requestVoteCodedIntoString) = 0;
	// 运行该机器，返回值是下一个状态
	virtual State* run();
};

