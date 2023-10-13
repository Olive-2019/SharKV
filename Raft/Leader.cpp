#include "Leader.h"
Leader::Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied) : 
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, ServerState::Leader, commitIndex, lastApplied) {
	ServerAddressReader serverAddressReader("AppendEntriesAddress.conf");
	serverAddress = serverAddressReader.getNetWorkAddresses();
	// ·¢ËÍĞÄÌø
}