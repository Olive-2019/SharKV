#include "KVserver.h"

KVserver::KVserver(NetWorkAddress raftServerAddress, int applyMsgPort, string snapshotFilePath): 
	raftServerAddress(raftServerAddress), snapshotPersistence(snapshotFilePath),debug(false) {
	//NetWorkAddress raftServerAddress("127.0.0.1", 8291);
	//Raft raft;
	setDebug();
	raft = new Raft(raftServerAddress, applyMsgPort, this);
	try {
		data = snapshotPersistence.read();
	}
	catch (exception e) {
		cout << "welcome to KV Server with Raft" << endl;
	}
}
KVserver::~KVserver() {
	snapshotPersistence.write(data);
	delete raft;
}
void KVserver::setDebug() {
	debug = true;
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
	if (debug) cout << "KVserver::execute " << endl;
	printState();
}
// 写快照
void KVserver::snapshot() {
	if (debug) cout << "KVserver::snapshot" << endl;
	snapshotPersistence.write(data);
}
// 接收命令，丢给Raft系统
void KVserver::acceptCommand(const Command& command) {
	raft->start(command);
}
bool KVserver::getData(int commandID, string& value) {
	
	if (readCache.find(commandID) == readCache.end()) return false;
	value = readCache[commandID];
	readCache.erase(commandID);
	return true;
}
void KVserver::printState() const {
	for (auto it = data.begin(); it != data.end(); ++it) 
		cout << it->first << ' ' << it->second << endl;
}
