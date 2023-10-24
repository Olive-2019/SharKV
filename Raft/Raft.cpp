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
	int currentTerm = -1, commitedIndex = -1, lastApplied = -1;
	vector<LogEntry> entries;
	state = new Follower(currentTerm, serverID, appendEntriesAddress, requestVoteAddress, 
		startAddress, commitedIndex, lastApplied, entries);
	while (state) {
		State* nextState = state->run();
		delete state;
		state = nextState;
	}
}