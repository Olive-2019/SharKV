#pragma once
#include "source.h"
#include "Command.h"
class RPC
{
public:
	// 调用底层kv数据库的acceptCommand函数，给kv数据库传命令
	int invokeRemoteAcceptCommand(NetWorkAddress address, Command command);
	// 调用shardCtrler获取新组地址
	NetWorkAddress invokeQueryGroupAddressByShardID(NetWorkAddress address, int shardID);
	// 调用shardCtrler获取自己的分配到的shard
	vector<int> invokeQueryShardIDByGroupID(NetWorkAddress address, int groupID);
};

