#include "Raft.h"
#include "FileOperator.h"
#include "KVserver.h"
int main() {
	/*NetWorkAddress raftServerAddress("127.0.0.1", 8291);
	Raft raft(raftServerAddress, 8001);
	raft.run();*/
	FileOperator fo("fo.data");
	map<string, string> data;/*
	data["Lam"] = "55190906";
	data["somebody"] = "55190922";*/
	//fo.write(data);
	map<string, string> d = fo.read();
	cout << endl;

}