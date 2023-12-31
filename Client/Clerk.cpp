#include "Clerk.h"

Clerk::Clerk(NetWorkAddress kvServerAddress) : kvServerAddress(kvServerAddress){

}

void Clerk::put(string key, string value) {
	Command command(CommandType::Put, key, value);
	rpc.invoke
	kvServer.acceptCommand(command);
}
void Clerk::append(string key, string value) {
	Command command(CommandType::Append, key, value);
	kvServer.acceptCommand(command);
}
// 同步阻塞的get
string Clerk::get(string key) {
	int commandID = aget(key);
	string value;
	while (true) {
		if (kvServer.getData(commandID, value)) break;
	}
	return value;
}
// 异步的get
int Clerk::aget(string key) {
	Command command(CommandType::Get, key);
	kvServer.acceptCommand(command);
	return command.getID();
}