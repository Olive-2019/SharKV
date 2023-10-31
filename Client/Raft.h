#pragma once
#include "StartAnswer.h"
#include "ApplyMsg.h"
using namespace rest_rpc;
class Raft
{
	// 连接raft集群的地址
	NetWorkAddress raftServerAddress;
	// 接收applyMsg的端口号
	int applyMsgPort;
	// 命令集
	vector<string> commands;
	// 最近被commit的命令下标
	int commitedIndex;
	// 接收applyMsg的线程
	thread* applyMsgThread;
	// debug标志位
	bool debug;

	// 执行到newCommitIndex
	void execute(int newCommitIndex);
	// 写快照
	void snapshot();
	// 更新命令，比较新命令和原有命令的区别，若不同则覆写并更新commitedIndex，commitedIndex = min(commitedIndex, the first index is different from new commands)
	void updateCommands(vector<string> newCommands);
public:
	Raft(NetWorkAddress raftServerAddress, int applyMsgPort);
	~Raft();
	// 向Raft系统发送命令
	// 阻塞函数，需要等待返回值
	StartAnswer start(string command);
	// 接收Raft系统的applyMsg信息
	void applyMsg(rpc_conn conn, ApplyMsg applyMsg);
	// 注册接收返回commit信息的函数
	void registerApplyMsg();
	void setDebug();
	void run();
};

