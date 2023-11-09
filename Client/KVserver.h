#pragma once
#include "source.h"
#include "Command.h"
#include "SnapshotPersistence.h"
#include "Raft.h"
class KVserver
{
	map<string, string> data;
	SnapshotPersistence snapshotPersistence;
	Raft* raft;
	NetWorkAddress raftServerAddress;
	map<int, string> readCache;
public:
	KVserver(NetWorkAddress raftServerAddress, int applyMsgPort = 8001, string snapshotFilePath = "snapshot.data");
	// 真正执行命令，由Raft调用
	void execute(const Command& command);
	// 写快照
	void snapshot();
	// 接收命令，丢给Raft系统
	void acceptCommand(const Command& command);
};

