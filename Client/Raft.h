#pragma once
#include "StartAnswer.h"
#include "ApplyMsg.h"
#include "source.h"
#include "Command.h"
#include "KVserver.h"
using namespace rest_rpc;
class Raft
{
	// 连接raft集群的地址
	NetWorkAddress raftServerAddress;
	// 接收applyMsg的端口号
	int applyMsgPort;
	// 命令集
	vector<Command> commands;
	// 最近被commit的命令下标
	int commitedIndex;
	// 接收applyMsg的线程
	thread* applyMsgThread;
	// debug标志位
	bool debug;
	// applyMsg锁，保证系统状态不被并行修改（commands、commitedIndex）
	mutex stateLock;

	// 执行到newCommitIndex
	void execute(int newCommitIndex);
	// 写快照
	void snapshot(int snapshotIndex);
	// 更新命令，比较新命令和原有命令的区别，若不同则覆写并更新commitedIndex，commitedIndex = min(commitedIndex, the first index is different from new commands)
	void updateCommands(vector<Command> newCommands);
	// 注册接收返回commit信息的函数
	void registerApplyMsg();
	// 开启调试信息
	void setDebug();

	// KV数据库指针
	KVserver* kvServer;
public:
	Raft(NetWorkAddress raftServerAddress, int applyMsgPort, KVserver* kvServer);
	~Raft();
	// 向Raft系统发送命令,阻塞函数，需要等待返回值
	StartAnswer start(Command command);
	// 接收Raft系统的applyMsg信息
	void applyMsg(rpc_conn conn, ApplyMsg applyMsg);
	void run();
};