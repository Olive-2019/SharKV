#include "Raft.h"
// 供client调用，向状态机发送命令，返回值是index和term，以空格间隔
string Raft::start(string command) {
	return "";
}
// 完成后向client发送可以执行该命令的请求
void Raft::applyMsg(string command, int index) {

}
// 运行状态机，调用状态机接口切换状态
void Raft::run(int serverID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress) {
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
		startAddress, commitedIndex, lastApplied, entries, votedFor);
	while (state) {
		State* nextState = state->run();
		delete state;
		state = nextState;
	}
}