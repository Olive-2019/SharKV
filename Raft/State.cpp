#include "State.h"

State::State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	ServerState state, int commitIndex, int lastApplied, vector<LogEntry> logEntries):
	currentTerm(currentTerm), ID(ID), appendEntriesAddress(appendEntriesAddress), state(state),
	requestVoteAddress(requestVoteAddress), commitIndex(commitIndex), lastApplied(lastApplied), 
	logEntries(logEntries){
	// 投票情况置为-1，即谁都没投
	votedFor = -1;
	// 读入集群中所有server的地址
	ServerAddressReader serverAddressReader("AppendEntriesAddress.conf");
	serverAddress = serverAddressReader.getNetWorkAddresses();
}

void State::timeoutCounterThread() {
	timeoutCounter.run();
	// 将几个线程里执行的指针置空
}

// 注册等待接收AppendEntries
void State::registerAppendEntries() {

}

// 注册投票线程RequestVote
void State::registerRequestVote() {

}