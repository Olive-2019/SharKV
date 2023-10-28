//#include "source.h"
#include "Raft.h"
NetWorkAddress getNetWorkAddress(const char* ip,  const char* port) {
	NetWorkAddress address;
	address.first = string(ip);
	address.second = atoi(port);
	return address;
}
int main(int argc, char* argv[]) {
	int serverID; 
	vector<NetWorkAddress> addresses;

	
	if (argc > 1) {
		cout << argc << endl;
		serverID = atoi(argv[1]);
		// 读入命令行参数
		for (int i = 0; i < 3; ++i) {
			cout << 2 + i * 2 << ' ' << 2 + i * 2 + 1 << endl;
			cout << argv[2 + i * 2] << " " << argv[2 + i * 2 + 1] << endl;
			addresses.push_back(getNetWorkAddress(argv[2 + i * 2], argv[2 + i * 2 + 1]));
		}
		addresses.push_back(getNetWorkAddress("127.0.0.1", "8001"));
	}
	else {
		// 调试用传参
		serverID = 1;
		addresses.push_back(getNetWorkAddress("127.0.0.1", "8091"));
		addresses.push_back(getNetWorkAddress("127.0.0.1", "8191"));
		addresses.push_back(getNetWorkAddress("127.0.0.1", "8291"));
		addresses.push_back(getNetWorkAddress("127.0.0.1", "8001"));
	}
	
	Raft raft;
	try {
		raft.run(serverID, addresses[0], addresses[1], addresses[2], addresses[3]);
	}
	catch (exception e) {
		cout << e.what() << endl;
	}
	
	
	return 0;
}