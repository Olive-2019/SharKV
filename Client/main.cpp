#include "Raft.h"
int main() {
	NetWorkAddress raftServerAddress("127.0.0.1", 8291);
	Raft raft(raftServerAddress, 8001);
	
}