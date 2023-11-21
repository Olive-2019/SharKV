#include "ShardCtrler.h"
ShardCtrler::ShardCtrler(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress, const int shardNum) 
	: clerk(kvServerAddress, getDataAddress), shardNum(shardNum), groupNum(0) {

}
// 获取系统中运行中的group
vector<int> ShardCtrler::getGroupID() {
	vector<int> groupIDs;
	for (int groupID = 1; groupID <= groupNum; ++groupID) 
		if (clerk.get(codeGroupID(groupID)).size()) groupIDs.push_back(groupID);
	return groupIDs;
}
// 重新生成config，即重新为shard指定group
void ShardCtrler::redistribute() {
	vector<int> groupIDs = getGroupID();
	int groupIDIndex = 0;
	for (int shardID = 0; shardID < shardNum; ++shardID) {
		move(shardID, groupIDs[groupIDIndex++]);
		groupIDIndex %= groupIDs.size();
	}
}
//Join : 新加入的Group信息（PUT、Append）,返回唯一标识groupid
int ShardCtrler::join(NetWorkAddress groupAdress) {
	int groupID = ++groupNum;
	clerk.put(codeGroupID(groupID), groupAdress.first + "_" + to_string(groupAdress.second));
	// 重新分配shard的位置
	redistribute();
	return groupID;
}
int ShardCtrler::join(rpc_conn conn, NetWorkAddress groupAdress) {
	return join(groupAdress);
}
//Leave : 哪些Group要离开（Delete）
void ShardCtrler::deleteGroup(rpc_conn conn, int groupID) {
	return deleteGroup(groupID);
}
void ShardCtrler::deleteGroup(int groupID) {
	clerk.deleteData(codeGroupID(groupID));
}
//Move : 将Shard分配给GID的Group, 无论它原来在哪（Delete Put）
void ShardCtrler::move(int shardID, int groupID) {
	clerk.put(codeShardID(shardID), codeGroupID(groupID));
}


//Query : 查询最新的Config信息（Get）
// 供client使用，获取key对应的分片数据库集群ip
NetWorkAddress ShardCtrler::queryByKey(rpc_conn conn, string key) {
	return queryByKey(key);
}
NetWorkAddress ShardCtrler::queryByKey(string key) {
	int shardID = getShardID(key);
	return queryGroupAddressByShardID(shardID);
}
// 供ShardKV使用，获取groupid对应的shardIDs
vector<int> ShardCtrler::queryShardIDByGroupID(rpc_conn conn, int groupID) {
	return queryShardIDByGroupID(groupID);
}
vector<int> ShardCtrler::queryShardIDByGroupID(int groupID) {
	vector<int> shardIDs;
	for (int shardID = 0; shardID < shardNum; ++shardID) {
		int curGroupID = getGroupID(clerk.get(codeShardID(shardID)));
		if (curGroupID == groupID) shardIDs.push_back(shardID);
	}
	return shardIDs;
}
// 供ShardKV使用，获取shardID对应的group地址
NetWorkAddress ShardCtrler::queryGroupAddressByShardID(rpc_conn conn, int shardID) {
	return queryGroupAddressByShardID(shardID);
}
NetWorkAddress ShardCtrler::queryGroupAddressByShardID(int shardID) {
	string groupIDStr = clerk.get(codeShardID(shardID));
	string address = clerk.get(groupIDStr);
	int len_ = address.find('_');
	string ip = address.substr(0, len_);
	int port = atoi(address.substr(len_).c_str());
	NetWorkAddress shardKVAddress(ip, port);
	return shardKVAddress;
}

// 计算该key对应的分片id
int ShardCtrler::getShardID(string key) {
	if (!key.size()) return 0;
	return key[0] % shardNum;
}
int ShardCtrler::getGroupID(string groupID) {
	return atoi(groupID.c_str()) * -1;
}
int ShardCtrler::getShardID(string shardID) {
	return atoi(shardID.c_str());
}
string ShardCtrler::codeGroupID(int groupID) {
	return to_string(groupID * -1);
}
string ShardCtrler::codeShardID(int shardID) {
	return to_string(shardID);
}