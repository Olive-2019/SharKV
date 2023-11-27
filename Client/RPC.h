#pragma once
#include "source.h"
#include "Command.h"
class RPC
{
public:
	// 向Clerk传输数据
	void invokeGetData(NetWorkAddress address, int commandID, string value);
	// 向KVCtrler申请join
	int invokeJoin(NetWorkAddress address, NetWorkAddress selfAddress);
	// 向KVCtrler索要当前group对应的shardid
	vector<int> invokeQueryShardID(NetWorkAddress address, int groupID);
	// 向KVCtrler索要当前shardid对应的新地址
	NetWorkAddress invokeQueryNewGroup(NetWorkAddress address, int shardID);
	// 调用底层kv数据库的acceptCommand函数，给kv数据库传命令
	int invokeRemoteAcceptCommand(NetWorkAddress address, Command command);
};

