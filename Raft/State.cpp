#include "State.h"

State::State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	ServerState state, int commitIndex, int lastApplied): 
	currentTerm(currentTerm), ID(ID), appendEntriesAddress(appendEntriesAddress), state(state),
	requestVoteAddress(requestVoteAddress), commitIndex(commitIndex), lastApplied(lastApplied) {
	votedFor = -1;
}

void State::timeoutCounterThread() {
	timeoutCounter.run();
	// 将几个线程里执行的指针置空
}
// 等待接收AppendEntries
string State::appendEntries(string appendEntriesCodedIntoString) {

}
// 注册等待接收AppendEntries
void State::registerAppendEntries() {

}
// 投票线程RequestVote
string State::requestVote(string requestVoteCodedIntoString) {

}
// 注册投票线程RequestVote
void State::registerRequestVote() {

}