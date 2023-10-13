#include "ServerAddressReader.h"
ServerAddressReader::ServerAddressReader(string filePath) : FileOperator(filePath) {}

pair<int, NetWorkAddress> ServerAddressReader::getOneNetWorkAddress() {
	string buff = getOneRaw();
	stringstream ss(buff);
	int ID;
	NetWorkAddress networkAddress;
	ss >> ID;
	ss >> networkAddress.first;
	ss >> networkAddress.second;
	return pair<int, NetWorkAddress>(ID, networkAddress);
}
map<int, NetWorkAddress> ServerAddressReader::getNetWorkAddresses() {
	openFile(std::ios::in);
	map<int, NetWorkAddress> IDToNetWorkAddress;
	while (isEnd()) {
		pair<int, NetWorkAddress> oneAddress = getOneNetWorkAddress();
		IDToNetWorkAddress.insert(oneAddress);
	}
	return IDToNetWorkAddress;
}