#pragma once
#include "State.h"

class Leader : public State
{
	// 需要发给每一个follower的下一条log entry（初始化值是leader的最后一个log entry的下一个值）
	map<int, int> nextIndex;
	// 每个follower当前匹配到哪一条log entry（初始化值为0）
	map<int, int> matchIndex;
	
	// 用于异步接收心跳/返回值的future
	map<int, vector<shared_future<Answer>>> followerReturnVal;
	// 最大重发次数
	int maxResendNum;

	// 记录上一个包，方便重发
	map<int, AppendEntries> lastAppendEntries;

	

	

	// 主线程：检测 发送appendEntries commit
	void work();
	// 更新commitIndex
	void updateCommit();
	// 发送更新的commit信息
	void applyMsg(bool snapshot = false, int snapshotIndex = -1);


	// 检测所有follower，重发或新发包
	void checkFollowers();
	// 检查单个follower，若成功则true，若不成功则尝试重发
	bool checkOneFollowerReturnValue(int followerID);
	// 获取单个follower的返回值
	Answer getOneFollowerReturnValue(int followerID);

	// 给指定ID的follower发送appendEntries，内容为本状态机的[start,end]的内容，若start<0则为空的心跳信息(组装好AppendEntries)
	void sendAppendEntries(int followerID, int start, int end, bool snapshot = false);
	// 发指定follower的包，返回值代表还能不能发包（拿一步的AppendEntries重发）
	bool sendAppendEntries(int followerID);

	// 注册等待接收AppendEntries句柄
	void registerHandleAppendEntries();
	// 注册投票线程RequestVote句柄
	void registerHandleRequestVote();
	// 注册start函数句柄
	void registerHandleStart();

	// 快照接口
	void snapShot();
	// 阻塞，通知follower写快照
	// 得自己开一条信道，不能跟原有的
	void informSnapshot(int snapshotIndex);
	// 快照对系统状态的改变
	void snapShotModifyState(int snapshotIndex);
	
public:
	Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, 
		vector<LogEntry> logEntries, int votedFor = -1, int maxResendNum = 3);
	// 析构函数完成线程join和delete掉线程对象的任务
	~Leader();
	// 接收RequestVote
	Answer requestVote(rpc_conn conn, RequestVote requestVote);
	// 接收AppendEntries
	Answer appendEntries(rpc_conn conn, AppendEntries appendEntries);
};

