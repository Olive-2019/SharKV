#pragma once
#include "source.h"
#include "LogEntry.h"
#include "TimeoutCounter.h"
#include "ServerAddressReader.h"
#include "RequestVote.h"
#include "AppendEntries.h"
#include "Answer.h"
#include <thread>
#include "RPC.h"
#include "../rest_rpc/include/rest_rpc.hpp"
using namespace rest_rpc::rpc_service;
using std::unique_ptr;
using namespace rest_rpc;
using std::thread;
using std::lock_guard;
using std::future_status;

class State
{
	// 接收句柄数量
	int handleNum;
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

	// 状态机用于接收start的ip和port
	NetWorkAddress startAddress;
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
	
	// AppendEntries线程
	thread* appendEntriesThread;
	// RequestVote线程
	thread* requestVoteThread;
	// 接收start信息的线程指针
	thread* startThread;

	/*控制线程的智能指针*/
	// 用于控制接收AppendEntries线程
	unique_ptr<rpc_server> startRpcServer;
	// 用于控制接收AppendEntries线程
	unique_ptr<rpc_server> appendEntriesRpcServer;
	// 用于控制接收RequestVote线程
	unique_ptr<rpc_server> requestVoteRpcServer;

	// 下一个状态，非空说明要进入下一个状态了，马上跳出循环，stop
	State* nextState;
	// 接收信息的大锁，进程中同时仅允许处理一条接收的信息，包括appendEntries和requestVote
	mutex receiveInfoLock;
	// 发送rpc信息
	RPC rpc;

	// 注册三个服务
	void registerServer();
	// 注册start函数
	void registerStart();
	// 注册等待接收AppendEntries
	void registerAppendEntries();
	// 注册投票线程RequestVote
	void registerRequestVote();

	
	// 添加entries，返回值表示是否成功添加
	bool appendEntriesReal(int prevLogIndex, int prevLogTerm, int leaderCommit, vector<LogEntry> entries);
	// 主线程 leader和candidate发东西，follower守护
	virtual void work() = 0;

	// debug输出开关
	bool debug;
public:
	// 构造函数完成初始化两个接收线程和计时器线程的任务
	State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,NetWorkAddress requestVoteAddress, 
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor = -1, int handleNum = 10);
	// 析构函数完成线程join和delete掉线程对象的任务
	virtual ~State();

	// 开启debug模式
	void setDebug() { debug = true; };
	// 获取当前currentTerm
	int getCurrentTerm() const;

	// start调用，leader和candidate添加一条新的entries，follower转发给leader
	virtual void start(AppendEntries newEntries);

	// 等待接收AppendEntries
	virtual string appendEntries(string appendEntriesCodedIntoString) = 0;

	// 投票线程RequestVote
	virtual string requestVote(string requestVoteCodedIntoString) = 0;

	// 运行该机器，返回值是下一个状态
	virtual State* run();
};

