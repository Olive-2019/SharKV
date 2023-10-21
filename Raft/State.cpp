#include "State.h"

State::State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	 int commitIndex, int lastApplied, vector<LogEntry> logEntries):
	currentTerm(currentTerm), ID(ID), appendEntriesAddress(appendEntriesAddress),
	requestVoteAddress(requestVoteAddress), commitIndex(commitIndex), lastApplied(lastApplied), 
	logEntries(logEntries){
	// 投票情况置为-1，即谁都没投
	votedFor = -1;
	// 读入集群中所有server的地址
	ServerAddressReader serverAddressReader("AppendEntriesAddress.conf");
	serverAddress = serverAddressReader.getNetWorkAddresses();
}
State* State::run() {
	// 开启计时器
	timeoutThread = new thread(timeoutCounterThread, this);
	// 开启AppendEntries
	appendEntriesThread = new thread(registerAppendEntries, this);
	// 开启RequestVote
	requestVoteThread = new thread(registerRequestVote, this);
	return NULL;
}
void State::timeoutCounterThread() {
	timeoutCounter.run();
	// 将几个线程里执行的指针置空
	stopThread();
}

// 注册等待接收AppendEntries
void State::registerAppendEntries() {
	appendEntriesRpcServer.reset(new rpc_server(appendEntriesAddress.second, 6));
	appendEntriesRpcServer->register_handler("appendEntries", appendEntries);
	appendEntriesRpcServer->run();//启动服务端
	cout << "State::registerAppendEntries close AppendEntries" << endl;
}

// 注册投票线程RequestVote
void State::registerRequestVote() {
	requestVoteRpcServer.reset(new rpc_server(requestVoteAddress.second, 6));
	requestVoteRpcServer->register_handler("requestVote", requestVote);
	requestVoteRpcServer->run();//启动服务端
	cout << "State::registerRequestVote close RequestVote" << endl;
}

void State::stopThread() {
	requestVoteRpcServer.reset(nullptr);
	appendEntriesRpcServer.reset(nullptr);
}
void State::waitThread() {
	timeoutThread->join();
	appendEntriesThread->join();
	requestVoteThread->join();
}