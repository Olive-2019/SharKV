#pragma once
#include "State.h"

class Leader : public State
{
	// 需要发给每一个follower的下一条log entry（初始化值是leader的最后一个log entry的下一个值）
	map<int, int> nextIndex;
	// 每个follower当前匹配到哪一条log entry（初始化值为0）
	map<int, int> matchIndex;
	// 用于异步接收心跳/返回值的future
	map<int, shared_future<string>> followerReturnVal;
	// 记录上一个包，方便重发
	map<int, AppendEntries> lastAppendEntries;

	

	

	// 主线程：检测 发送appendEntries commit
	void work();
	// 更新commitIndex
	void updateCommit();
	// 检测所有follower，重发或新发包
	void checkFollowers();

	// 给指定ID的follower发送appendEntries，内容为本状态机的[start,end]的内容，若start<0则为空的心跳信息
	void sendAppendEntries(int followerID, int start, int end);
	// 重发指定follower的包
	void resendAppendEntries(int followerID);
	
	
public:
	Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries);
	// 析构函数完成线程join和delete掉线程对象的任务
	~Leader();
	// 接收RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// 接收AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
	// 运行该机器，返回值是下一个状态
	State* run();
};

