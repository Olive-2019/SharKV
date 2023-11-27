#pragma once
#include "source.h"
#include "Command.h"
#include "SnapshotPersistence.h"
#include "Raft.h"
#include "RPC.h"
class Raft;
class KVserver
{
protected:

	string selfIP;
	map<string, string> data;
	SnapshotPersistence snapshotPersistence;
	Raft* raft;
	NetWorkAddress raftServerAddress;
	RPC rpc;
	// 打印调试信息的开关
	bool debug;
	void setDebug();
	// 给Clerk开放的接口
	int acceptCommandPort;
	// 注册接收Clerk调用的函数
	void registerAcceptCommand();
	// Clerk accept command 跑的线程
	thread* acceptCommandThread;
	// print the data in kv server
	void printState() const;
public:
	KVserver(NetWorkAddress raftServerAddress, int applyMsgPort = 8001, int acceptCommandPort = 8011, 
		string snapshotFilePath = "snapshot.data", string selfIP = "127.0.0.1");
	~KVserver();
	// 真正执行命令，由Raft调用
	virtual void execute(const Command& command);
	// 写快照，由Raft调用
	void snapshot();
	// 接收命令，丢给Raft系统
	int acceptCommand(rpc_conn conn, const Command& command);
	int acceptCommand_(const Command& command);
};

