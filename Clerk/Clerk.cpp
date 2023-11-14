#include "Clerk.h"

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

// 异步的get
int Clerk::aget(string key) {
	Command command(CommandType::Get, getDataAddress, key);
	invokeCommand(command);
	return command.getID();
}

// 返回数据调用接口
void Clerk::getData(rpc_conn conn, int commandID, string value) {
	cout << "Clerk::getData commandID " << commandID << " value " << value << endl;
}