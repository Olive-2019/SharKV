#pragma once
#include "StartAnswer.h"
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
public:
	Raft(NetWorkAddress raftServerAddress, int applyMsgPort);
	~Raft();
	// 向Raft系统发送命令
	// 阻塞函数，需要等待返回值
	StartAnswer start(string command);
	// 接收Raft系统的applyMsg信息
	void applyMsg(rpc_conn conn, string command, int index);
	// 注册接收返回commit信息的函数
	void registerApplyMsg();
	void setDebug();
	void run();
};

