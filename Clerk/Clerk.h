#pragma once
#include "source.h"
#include "RPC.h"
#include <atomic>
using std::atomic;
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
	//返回数据
	map<int, string> dataCache;
	//mutex dataCacheLock;
public:
	Clerk(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress);
	~Clerk();
	// 添加数据
	void put(string key, string value);
	// 向某个key追加数据
	void append(string key, string value);
	// 删除数据项
	void deleteData(string key);
	// 同步的get
	string get(string key);
	// 返回数据调用接口
	void getData(rpc_conn conn, int commandID, string value);
	// 异步的get，返回commandID
	int aget(string key);
	
};
