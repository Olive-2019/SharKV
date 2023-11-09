#pragma once
#include "KVserver.h"
class Clerk
{
	KVserver kvServer;
public:
	Clerk(NetWorkAddress raftServerAddress, int applyMsgPort = 8001, string snapshotFilePath = "snapshot.data");
	void put(string key, string value);
	void append(string key, string value);
	// 同步阻塞的get
	string get(string key);
	// 异步的get，返回commandID
	int aget(string key);
};

