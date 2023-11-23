#include "ShardKV.h"
ShardKV::ShardKV(NetWorkAddress raftServerAddress, NetWorkAddress shardCtrlerJoinAddress, NetWorkAddress shardCtrlerQueryShardIDAddress,
	NetWorkAddress shardCtrlerQueryNewGroupAddress, int applyMsgPort, int acceptCommandPort, string snapshotFilePath, int shardNum)
	: KVserver(raftServerAddress, applyMsgPort, acceptCommandPort, snapshotFilePath), shardNum(shardNum), shardCtrlerJoinAddress(shardCtrlerJoinAddress),
	shardCtrlerQueryNewGroupAddress(shardCtrlerQueryNewGroupAddress), shardCtrlerQueryShardIDAddress(shardCtrlerQueryShardIDAddress), groupID(-1){
	for (int i = 0; i < shardNum; ++i) shardIDs.insert(i);
	join();
}
int ShardKV::getShardID(string key) {
	// 计算该key对应的分片id
	if (!key.size()) return 0;
	return key[0] % shardNum;
}

void ShardKV::execute(const Command& command) {
	int shardID = getShardID(command.getKey());
	if (debug) cout << "ShardKV::execute accept a comand with shard " << shardID << endl;
	if (shardIDs.find(shardID) == shardIDs.end()) return;
	if (command.getType() == CommandType::AddShard) shardIDs.insert(atoi(command.getKey().c_str()));
	else if (command.getType() == CommandType::PutShard) data[command.getKey()] = command.getValue();
	else if (shardIDs.find(shardID) != shardIDs.end()) KVserver::execute(command);
}

// 检查失去了那些shard
set<int> ShardKV::checkDeleted(vector<int> newShardIDs) {

}
// 发送Put请求
void ShardKV::sendPutShardWithShardID(NetWorkAddress address, int shardID) {

}
// 发送AddShard请求
void ShardKV::sendAddShard() {

}
// 拉取配置信息
void ShardKV::getConfig() {

}
// 向shardCtrler登记join
void ShardKV::join() {
	NetWorkAddress selfAddress(to_string)
}
