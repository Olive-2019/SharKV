#pragma once
#include "source.h"
class ShardCtrler
{
	// 分片数量
	const int shardNum;
	// 分组数量
	int groupNum;

	// 计算该key对应的分片id
	int getShardID(string key);

	// 获取系统中运行中的group
	vector<int> getGroupID();

	// 重新分配
	void redistribute();

	// ID编解码
	int decodeGroupID(string groupID);
	int decodeShardID(string shardID);
	string codeGroupID(int groupID);
	string codeShardID(int shardID);

	// server线程
	vector<thread*> threads;
	vector<int> ports;
	bool debug;


public:
	ShardCtrler(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress, const int shardNum, vector<int> ports);
	~ShardCtrler();
	void regsiterFunc(int index);
	//Join : 新加入的Group信息（PUT、Append）,返回唯一标识groupid
	int join_(NetWorkAddress groupAdress);
	int join(rpc_conn conn, NetWorkAddress groupAdress);
	//Leave : 哪些Group要离开（Delete）
	void deleteGroup_(int groupID);
	void deleteGroup(rpc_conn conn, int groupID);
	//Move : 将Shard分配给GID的Group, 无论它原来在哪（Delete Put）
	void move(int shardID, int groupID);
	//void move(rpc_conn conn, int shardID, int groupID);


	// 调试用，打印现在有多少shard，及其对应的group
	void printShardDistribution();
	//Query : 查询最新的Config信息（Get）
	// 供client使用，获取key对应的分片数据库集群ip
	NetWorkAddress queryByKey_(string key);
	NetWorkAddress queryByKey(rpc_conn conn, string key);
	// 供ShardKV使用，获取groupid对应的shardIDs
	vector<int> queryShardIDByGroupID_(int groupID);
	vector<int> queryShardIDByGroupID(rpc_conn conn, int groupID);
	// 供ShardKV使用，获取shardID对应的group地址
	NetWorkAddress queryGroupAddressByShardID_(int shardID);
	NetWorkAddress queryGroupAddressByShardID(rpc_conn conn, int shardID);

};

