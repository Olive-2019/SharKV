#include "ShardCtrler.h"
ShardCtrler::ShardCtrler(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress, const int shardNum, vector<int> ports)
	: clerk(kvServerAddress, getDataAddress), shardNum(shardNum), groupNum(0), ports(ports), debug(true) {
	for (int i = 0; i < ports.size(); ++i) {
		threads.push_back(new thread(&ShardCtrler::regsiterFunc, this, i));
	}
}
ShardCtrler::~ShardCtrler() {
	for (int i = 0; i < ports.size(); ++i) {
		threads[i]->join();
		delete threads[i];
		threads[i] = NULL;
	}
}
void ShardCtrler::regsiterFunc(int index) {
	vector<string> funcName = { "join", "deleteGroup", "queryByKey", "queryShardIDByGroupID", "queryGroupAddressByShardID" };
	rpc_server server(ports[index], 6);
	switch (index) {
	case 0:
		server.register_handler(funcName[index].c_str(), &ShardCtrler::join, this);
		break;
	case 1:
		server.register_handler(funcName[index].c_str(), &ShardCtrler::deleteGroup, this);
		break;
	case 2:
		server.register_handler(funcName[index].c_str(), &ShardCtrler::queryByKey, this);
		break;
	case 3:
		server.register_handler(funcName[index].c_str(), &ShardCtrler::queryShardIDByGroupID, this);
		break;
	case 4:
		server.register_handler(funcName[index].c_str(), &ShardCtrler::queryGroupAddressByShardID, this);
		break;
	}
	server.run();
}


// 获取系统中运行中的group
vector<int> ShardCtrler::getGroupID() {
	vector<int> groupIDs;
	if (debug) cout << "ShardCtrler::getGroupID" << endl;
	for (int groupID = 1; groupID <= groupNum; ++groupID) {
		string groupAddress = clerk.get(codeGroupID(groupID));
		if (groupAddress.size()) groupIDs.push_back(groupID);
		if (debug) {
			cout << "groupID " << groupID << " groupAddress " << groupAddress << endl;
		}
	}
	return groupIDs;
}
void ShardCtrler::printShardDistribution()  {
	if (!debug) return;
	cout << "ShardCtrler::printShardDistribution" << endl;
	for (int shardID = 0; shardID < shardNum; ++shardID) {
		int groupID = decodeGroupID(clerk.get(codeShardID(shardID)));
		cout << "shardID " << shardID << " groupID " << groupID << endl;
	}
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
int ShardCtrler::join_(NetWorkAddress groupAdress) {
	int groupID = ++groupNum;
	clerk.put(codeGroupID(groupID), groupAdress.first + "_" + to_string(groupAdress.second));
	// 重新分配shard的位置
	redistribute();
	return groupID;
}
int ShardCtrler::join(rpc_conn conn, NetWorkAddress groupAdress) {
	return join_(groupAdress);
}
//Leave : 哪些Group要离开（Delete）
void ShardCtrler::deleteGroup(rpc_conn conn, int groupID) {
	return deleteGroup_(groupID);
}
void ShardCtrler::deleteGroup_(int groupID) {
	clerk.deleteData(codeGroupID(groupID));
}
//Move : 将Shard分配给GID的Group, 无论它原来在哪（Delete Put）
void ShardCtrler::move(int shardID, int groupID) {
	clerk.put(codeShardID(shardID), codeGroupID(groupID));
}


//Query : 查询最新的Config信息（Get）
// 供client使用，获取key对应的分片数据库集群ip
NetWorkAddress ShardCtrler::queryByKey(rpc_conn conn, string key) {
	return queryByKey_(key);
}
NetWorkAddress ShardCtrler::queryByKey_(string key) {
	int shardID = getShardID(key);
	if (debug) cout << "ShardCtrler::queryByKey_ shardID " << shardID << endl;
	return queryGroupAddressByShardID_(shardID);
}
// 供ShardKV使用，获取groupid对应的shardIDs
vector<int> ShardCtrler::queryShardIDByGroupID(rpc_conn conn, int groupID) {
	return queryShardIDByGroupID_(groupID);
}
vector<int> ShardCtrler::queryShardIDByGroupID_(int groupID) {
	vector<int> shardIDs;
	for (int shardID = 0; shardID < shardNum; ++shardID) {
		int curGroupID = decodeGroupID(clerk.get(codeShardID(shardID)));
		if (curGroupID == groupID) shardIDs.push_back(shardID);
	}
	return shardIDs;
}
// 供ShardKV使用，获取shardID对应的group地址
NetWorkAddress ShardCtrler::queryGroupAddressByShardID(rpc_conn conn, int shardID) {
	return queryGroupAddressByShardID_(shardID);
}
NetWorkAddress ShardCtrler::queryGroupAddressByShardID_(int shardID) {
	string groupIDStr = clerk.get(codeShardID(shardID));
	string address = clerk.get(groupIDStr);
	int len_ = address.find('_');
	string ip = address.substr(0, len_);
	string portStr = address.substr(len_ + 1, address.size() - len_);
	int port = atoi(portStr.c_str());
	NetWorkAddress shardKVAddress(ip, port);
	return shardKVAddress;
}

// 计算该key对应的分片id
int ShardCtrler::getShardID(string key) {
	if (!key.size()) return 0;
	return key[0] % shardNum;
}
int ShardCtrler::decodeGroupID(string groupID) {
	return atoi(groupID.c_str()) * -1;
}
int ShardCtrler::decodeShardID(string shardID) {
	return atoi(shardID.c_str());
}
string ShardCtrler::codeGroupID(int groupID) {
	return to_string(groupID * -1);
}
string ShardCtrler::codeShardID(int shardID) {
	return to_string(shardID);
}