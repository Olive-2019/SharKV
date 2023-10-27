#pragma once
#include "State.h"

class Leader : public State
{
	// 需要发给每一个follower的下一条log entry（初始化值是leader的最后一个log entry的下一个值）
	map<int, int> nextIndex;
	// 每个follower当前匹配到哪一条log entry（初始化值为0）
	map<int, int> matchIndex;
	
	// 用于异步接收心跳/返回值的future
	map<int, vector<shared_future<string>>> followerReturnVal;
	// 最大重发次数
	int maxResendNum;

	// 记录上一个包，方便重发
	map<int, AppendEntries> lastAppendEntries;

	

	

	// 主线程：检测 发送appendEntries commit
	void work();
	// 更新commitIndex
	void updateCommit();
	// 检测所有follower，重发或新发包
	void checkFollowers();
	// 检查单个follower，若成功则true，若不成功则尝试重发
	bool checkOneFollowerReturnValue(int followerID);
	// 获取单个follower的返回值
	Answer getOneFollowerReturnValue(int followerID);

	// 给指定ID的follower发送appendEntries，内容为本状态机的[start,end]的内容，若start<0则为空的心跳信息(组装好AppendEntries)
	void sendAppendEntries(int followerID, int start, int end);
	// 发指定follower的包，返回值代表还能不能发包（拿一步的AppendEntries重发）
	bool sendAppendEntries(int followerID);
	
	
public:
	Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries,
		int votedFor = -1, int maxResendNum = 3);
	// 析构函数完成线程join和delete掉线程对象的任务
	~Leader();
	// 接收RequestVote
	string requestVote(string requestVoteCodedIntoString);
	// 接收AppendEntries
	string appendEntries(string appendEntriesCodedIntoString);
};

