#pragma once
#include "State.h"

class Candidate : public State
{

	// 用于计算超时的类
	TimeoutCounter timeoutCounter;
	// 计时器线程
	thread* timeoutThread;
	// 计算超时的线程，结束时会把其他几个接收线程都退出，所以如果想要结束当前所有线程，可以调用结束计时器的函数
	void timeoutCounterThread();

	// 投票结果 0:没有收到 1：赢得该选票 -1：输了
	map<int, int> voteResult;
	// 获得的选票数量
	int getVoteCounter;
	// 被拒绝的次数
	int rejectCounter;
	
	// 主线程：运行检测投票
	void work();
	// 检查发送的投票信息，返回值为true：都返回了 false：存在未返回的值
	bool checkRequestVote();
	// 根据当前信息，尝试发送requestVote，返回值表示是否发送成功
	bool sendRequestVote(int followerID);

	// 检查单个follower，若成功则true，若不成功则尝试重发
	bool checkOneFollowerReturnValue(int followerID, Answer& ans);


	// 检测投票结果，只有返回true时知道选举成功，返回false意味着未知数，选举失败靠appendEntries
	bool checkVoteResult();

	// 用于异步接收心跳/返回值的future
	map<int, vector<shared_future<Answer>>> followerReturnVal;
	// 最大的重发次数
	int maxResendNum;

	// 注册等待接收AppendEntries句柄
	void registerHandleAppendEntries();
	// 注册投票线程RequestVote句柄
	void registerHandleRequestVote();
	// 注册start函数句柄
	void registerHandleStart();
public:
	Candidate(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries,
		int votedFor = -1, int maxResendNum = 3);
	~Candidate();
	// 接收RequestVote
	Answer requestVote(rpc_conn conn, RequestVote requestVote);
	// 接收AppendEntries
	Answer appendEntries(rpc_conn conn, AppendEntries appendEntries);

};

