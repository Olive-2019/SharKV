#include "KVserver.h"

KVserver::KVserver(NetWorkAddress raftServerAddress, int applyMsgPort, int acceptCommandPort, string snapshotFilePath):
	raftServerAddress(raftServerAddress), snapshotPersistence(snapshotFilePath),debug(false), acceptCommandPort(acceptCommandPort) {
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
	acceptCommandThread = new thread(&KVserver::registerAcceptCommand, this);
}
KVserver::~KVserver() {
	snapshotPersistence.write(data);
	delete raft;
	acceptCommandThread->join();
	delete acceptCommandThread;
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
		string value;
		if (data.find(command.getKey()) != data.end()) value = data[command.getKey()];
		rpc.invokeGetData(command.getClerkAddress(), command.getID(), value);
	}
	if (debug) {
		cout << "KVserver::execute " << endl;
		printState();
		cout << endl;
	}
	

}
// 写快照
void KVserver::snapshot() {
	if (debug) cout << "KVserver::snapshot" << endl;
	snapshotPersistence.write(data);
}

void KVserver::registerAcceptCommand() {
	rpc_server server(acceptCommandPort, 6);
	server.register_handler("acceptCommand", &KVserver::acceptCommand, this);
	server.run();
}

// 接收命令，丢给Raft系统，返回下标
int KVserver::acceptCommand(rpc_conn conn, const Command& command) {
	StartAnswer startAnswer = raft->start(command);
	return startAnswer.index;
}

void KVserver::printState() const {
	for (auto it = data.begin(); it != data.end(); ++it) 
		cout << it->first << ' ' << it->second << endl;
}
