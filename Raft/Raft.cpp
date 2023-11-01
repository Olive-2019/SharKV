#include "Raft.h"

// 运行状态机，调用状态机接口切换状态
void Raft::run(int serverID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, NetWorkAddress applyMessageAddress) {
	// 读取持久化信息，若无持久化信息，则写入持久化信息
	PersistenceInfoReaderAndWriter persistenceInfoReaderAndWriter(serverID);
	persistenceInfoReaderAndWriter.read();
	int currentTerm = persistenceInfoReaderAndWriter.getCurrentTerm(), commitedIndex = -1;
	int votedFor = persistenceInfoReaderAndWriter.getVotedFor();
	vector<LogEntry> entries = persistenceInfoReaderAndWriter.getEntries();
	int lastApplied = entries.size() - 1;
	// 从Follower开始循环
	//currentTerm = 1;
	
	state = new Follower(currentTerm, serverID, appendEntriesAddress, requestVoteAddress, 
		startAddress, applyMessageAddress, commitedIndex, lastApplied, entries, votedFor);
	while (state) {
		State* nextState = state->run();
		delete state;
		if (!nextState) throw exception("Raft::run the next state is NULL.");
		state = nextState;
	}
}