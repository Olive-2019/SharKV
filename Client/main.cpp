//#include "Clerk.h"
#include "KVserver.h"
int main(int argc, char* argv[]) {
	NetWorkAddress raftServerAddress("127.0.0.1", 8291);
	int applyMsgPort = 8001, acceptCommandPort = 8011;
	if (argc > 1) {
		cout << argc << endl;
		// 读入命令行参数
		applyMsgPort = atoi(argv[1]);
		raftServerAddress = NetWorkAddress(string(argv[2]), atoi(argv[3]));
	}
	string snapshotFilePath = "snapshot" + to_string(applyMsgPort) + ".data";
	KVserver kvServer(raftServerAddress, applyMsgPort, acceptCommandPort, snapshotFilePath);
	
	return 0;
}