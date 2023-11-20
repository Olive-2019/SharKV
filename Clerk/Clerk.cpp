#include "Clerk.h"
//using namespace std::chrono::hh_mm_ss;

Clerk::Clerk(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress, vector<int> crudPorts) :
	kvServerAddress(kvServerAddress), getDataAddress(getDataAddress), crudPorts(crudPorts), crudThreads(4, NULL) {
	getDataThread = new thread(&Clerk::registerGetData, this);
	crudThreads[int(CommandType::Put)] = new thread(&Clerk::registerPut, this);
	crudThreads[int(CommandType::Delete)] = new thread(&Clerk::registerDelete, this);
	crudThreads[int(CommandType::Append)] = new thread(&Clerk::registerAppend, this);
	crudThreads[int(CommandType::Get)] = new thread(&Clerk::registerGet, this);
}
Clerk::~Clerk() {
	getDataThread->join();
	delete getDataThread;
	for (thread* crudThread : crudThreads) {
		crudThread->join();
		delete crudThread;
	}
}

void Clerk::registerPut() {
	rpc_server server(crudPorts[int(CommandType::Put)], 6);
	server.register_handler("put", &Clerk::put, this);
	server.run();
}
void Clerk::registerDelete() {
	rpc_server server(crudPorts[int(CommandType::Delete)], 6);
	server.register_handler("delete", &Clerk::deleteData, this);
	server.run();
}
void Clerk::registerAppend() {
	rpc_server server(crudPorts[int(CommandType::Append)], 6);
	server.register_handler("append", &Clerk::append, this);
	server.run();
}
void Clerk::registerGet() {
	rpc_server server(crudPorts[int(CommandType::Get)], 6);
	server.register_handler("get", &Clerk::get, this);
	server.run();
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

void Clerk::put(rpc_conn conn, string key, string value) {
	Command command(CommandType::Put, getDataAddress, key, value);
	invokeCommand(command);
}
void Clerk::append(rpc_conn conn, string key, string value) {
	Command command(CommandType::Append, getDataAddress, key, value);
	invokeCommand(command);
}
// 删除数据项
void Clerk::deleteData(rpc_conn conn, string key) {
	Command command(CommandType::Delete, getDataAddress, key);
	invokeCommand(command);
}
// 同步的get
string Clerk::get(rpc_conn conn, string key) {
	int commandID = aget(conn, key);
	while (dataCache.find(commandID) == dataCache.end()) {
		continue;
	}
	string value = dataCache[commandID];
	dataCache.erase(commandID);
	return value;
}
// 异步的get
int Clerk::aget(rpc_conn conn, string key) {
	Command command(CommandType::Get, getDataAddress, key);
	int commandID = invokeCommand(command);
	return commandID;
}

// 返回数据调用接口
void Clerk::getData(rpc_conn conn, int commandID, string value) {
	dataCache[commandID] = value;
	cout << "Clerk::getData commandID " << commandID << " value " << value << endl;
}