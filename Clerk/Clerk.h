#pragma once
#include "source.h"
#include "RPC.h"
class Clerk
{
	NetWorkAddress kvServerAddress;
	RPC rpc;
	int invokeCommand(Command command);
	// 返回数据的地址
	NetWorkAddress getDataAddress;
	thread* getDataThread;
	// 注册getData函数
	void registerGetData();
public:
	Clerk(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress);
	~Clerk();
	void put(string key, string value);
	void append(string key, string value);
	// 返回数据调用接口
	void getData(rpc_conn conn, int commandID, string value);
	// 异步的get，返回commandID
	int aget(string key);
};
