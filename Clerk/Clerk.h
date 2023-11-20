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
	// 增删改查服务地址
	vector<int> crudPorts;
	// 增删改查线程指针
	vector<thread*> crudThreads;
	// 注册增删改查四个服务
	void registerPut();
	void registerDelete();
	void registerAppend();
	void registerGet();


	//返回数据
	map<int, string> dataCache;
	//mutex dataCacheLock;
public:
	// 参数：kv数据库地址、get返回数据的地址（本机）、put delete append get的端口号
	Clerk(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress, vector<int> crudPort);
	~Clerk();
	// 添加数据
	void put(rpc_conn conn, string key, string value);
	// 向某个key追加数据
	void append(rpc_conn conn, string key, string value);
	// 删除数据项
	void deleteData(rpc_conn conn, string key);
	// 同步的get
	string get(rpc_conn conn, string key);
	// 返回数据调用接口
	void getData(rpc_conn conn, int commandID, string value);
	// 异步的get，返回commandID
	int aget(rpc_conn conn, string key);
	
};
