#include "Clerk.h"
int main(int argc, char* argv[]) {
	NetWorkAddress raftServerAddress("127.0.0.1", 8291);
	int applyMsgPort = 8001;
	if (argc > 1) {
		cout << argc << endl;
		// 读入命令行参数
		applyMsgPort = atoi(argv[1]);
		raftServerAddress = NetWorkAddress(string(argv[2]), atoi(argv[3]));
	}
	string snapshotFilePath = "snapshot" + to_string(applyMsgPort) + ".data";
	Clerk clerk(raftServerAddress, applyMsgPort, snapshotFilePath);

	clerk.put("Lam", "55190906");
	clerk.put("Lam", "55190906");
	clerk.put("Lam", "55190906");
	clerk.put("Lam", "55190906");
	clerk.put("somebody", "55190922");
	clerk.put("somebody", "55190922");
	clerk.put("somebody", "55190922");
	clerk.put("somebody", "55190922");
	clerk.put("somebody", "55190922");
	clerk.append("Lam", "123");
	clerk.append("Lam", "123");
	clerk.append("Lam", "123");
	clerk.append("Lam", "123");
	//cout << clerk.get("Lam") << endl;
	return 0;
}