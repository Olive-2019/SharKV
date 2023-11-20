#include "Clerk.h"
//using namespace std::chrono::hh_mm_ss;

Clerk::Clerk(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress) :
	kvServerAddress(kvServerAddress), getDataAddress(getDataAddress) {
	getDataThread = new thread(&Clerk::registerGetData, this);
}
Clerk::~Clerk() {
	getDataThread->join();
	delete getDataThread;
}
void Clerk::registerGetData() {
	rpc_server server(getDataAddress.second, 6);
	server.register_handler("getData", &Clerk::getData, this);
	server.run();
}

int Clerk::invokeCommand(Command command) {
	int commandID = rpc.invokeRemoteAcceptCommand(kvServerAddress, command);
	cout << "Clerk::invokeCommand " << commandID << endl;
	return commandID;
}

void Clerk::put(string key, string value) {
	Command command(CommandType::Put, getDataAddress, key, value);
	invokeCommand(command);
}
void Clerk::append(string key, string value) {
	Command command(CommandType::Append, getDataAddress, key, value);
	invokeCommand(command);
}
// 删除数据项
void Clerk::deleteData(string key) {
	Command command(CommandType::Delete, getDataAddress, key);
	invokeCommand(command);
}
// 同步的get
string Clerk::get(string key) {
	int commandID = aget(key);
	while (dataCache.find(commandID) == dataCache.end()) {
		continue;
	}
	string value = dataCache[commandID];
	dataCache.erase(commandID);
	return value;
}
// 异步的get
int Clerk::aget(string key) {
	Command command(CommandType::Get, getDataAddress, key);
	int commandID = invokeCommand(command);
	return commandID;
}

// 返回数据调用接口
void Clerk::getData(rpc_conn conn, int commandID, string value) {
	dataCache[commandID] = value;
	cout << "Clerk::getData commandID " << commandID << " value " << value << endl;
}