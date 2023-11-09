#include "KVserver.h"

KVserver::KVserver(NetWorkAddress raftServerAddress, int applyMsgPort, string snapshotFilePath): 
	raftServerAddress(raftServerAddress), snapshotPersistence(snapshotFilePath) {
	//NetWorkAddress raftServerAddress("127.0.0.1", 8291);
	//Raft raft;
	raft = new Raft(raftServerAddress, applyMsgPort, this);
	data = snapshotPersistence.read();
}
// 真正执行命令，由Raft调用
void KVserver::execute(const Command& command) {
	switch (command.getType()) {
	case CommandType::Append:
		// 数据库中存在则追加后跳出
		if (data.find(command.getKey()) != data.end()) {
			data[command.getKey()] += command.getValue();
			break;
		}
		// 数据库中不存在则进入put操作
	case CommandType::Put:
		// 写入
		data[command.getKey()] = command.getValue();
		break;
	default:
		// 数据库中不存在，则返回一个空字符串，存在则写到cache中去
		if (data.find(command.getKey()) == data.end()) readCache[command.getID()] = "";
		else readCache[command.getID()] = data[command.getKey()];
	}
}
// 写快照
void KVserver::snapshot() {
	snapshotPersistence.write(data);
}
// 接收命令，丢给Raft系统
void KVserver::acceptCommand(const Command& command) {
	raft->start(command);
}