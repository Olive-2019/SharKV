#pragma once
#include "KVserver.h"
class ShardKV :
    public KVserver
{
	// 系统中的shard数量，用于全局初始化
	const int shardNum;
	// 当前group的所有shard
	set<int> shardIDs;
	// 当前group的唯一标志
	int groupID;

	RPC rpc;


	// shardCtrler登记join的地址
	NetWorkAddress shardCtrlerJoinAddress;
	// shardCtrler查询所有shardID的接口地址
	NetWorkAddress shardCtrlerQueryShardIDAddress;
	// shardCtrler查询新group地址的接口地址
	NetWorkAddress shardCtrlerQueryNewGroupAddress;

	// 计算该key对应的分片id
	int getShardID(string key);
	// 检查失去了那些shard
	set<int> checkDeleted(vector<int> newShardIDs);
	// 发送Put请求
	void sendPutShardWithShardID(NetWorkAddress address, int shardID);
	// 发送AddShard请求
	void sendAddShard();
	// 拉取配置信息
	void getConfig();
	// 向shardCtrler登记join
	void join();
public:
	ShardKV(NetWorkAddress raftServerAddress, NetWorkAddress shardCtrlerJoinAddress, NetWorkAddress shardCtrlerQueryShardIDAddress,
		NetWorkAddress shardCtrlerQueryNewGroupAddress, int applyMsgPort = 8001, int acceptCommandPort = 8011, 
		string snapshotFilePath = "snapshot.data", int shardNum = 10);
	void execute(const Command& command);

};

