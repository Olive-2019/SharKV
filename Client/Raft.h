#pragma once
#include "StartAnswer.h"
using namespace rest_rpc;
class Raft
{
	NetWorkAddress raftServerAddress;
	int applyMsgPort;
	vector<string> commands;
	int commitedIndex;
	thread* applyMsgThread;
	bool debug;
public:
	Raft(NetWorkAddress raftServerAddress, int applyMsgPort);
	~Raft();
	// 向Raft系统发送命令
	// 阻塞函数，需要等待返回值
	StartAnswer start(string command);
	// 接收Raft系统的applyMsg信息
	bool applyMsg(string command, int index);
	// 注册接收返回commit信息的函数
	void registerApplyMsg();
	void setDebug();
	void run();
};

