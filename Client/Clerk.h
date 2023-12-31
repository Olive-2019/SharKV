#pragma once
#include "KVserver.h"
#include "RPC.h"
class Clerk
{
	// 连接kv数据库的地址
	NetWorkAddress kvServerAddress;
	RPC rpc;
public:
	Clerk(NetWorkAddress kvServerAddress);
	void put(string key, string value);
	void append(string key, string value);
	// 同步阻塞的get
	string get(string key);
	// 异步的get，返回commandID
	int aget(string key);
};

