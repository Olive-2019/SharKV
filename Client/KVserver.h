#pragma once
#include "source.h"
#include "Command.h"
#include "SnapshotPersistence.h"
#include "Raft.h"
class Raft;
class KVserver
{
	map<string, string> data;
	SnapshotPersistence snapshotPersistence;
	Raft* raft;
	NetWorkAddress raftServerAddress;
	map<int, string> readCache;
	bool debug;
	void setDebug();
public:
	KVserver(NetWorkAddress raftServerAddress, int applyMsgPort = 8001, string snapshotFilePath = "snapshot.data");
	~KVserver();
	// 真正执行命令，由Raft调用
	void execute(const Command& command);
	// 写快照
	void snapshot();
	// 接收命令，丢给Raft系统
	void acceptCommand(const Command& command);
	// 查询cache中的数据，如果查到了，即从cache中删除，返回值代表是否查到
	bool getData(int commandID, string& value);
	// print the data in kv server
	void printState() const;
};

