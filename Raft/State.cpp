#include "State.h"
#include "Candidate.h"
State::State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	 int commitIndex, int lastApplied, vector<LogEntry> logEntries):
	currentTerm(currentTerm), ID(ID), appendEntriesAddress(appendEntriesAddress),
	requestVoteAddress(requestVoteAddress), commitIndex(commitIndex), lastApplied(lastApplied), 
	logEntries(logEntries), nextState(NULL){
	// 投票情况置为-1，即谁都没投
	votedFor = -1;
	// 读入集群中所有server的地址
	ServerAddressReader serverAddressReader("AppendEntriesAddress.conf");
	serverAddress = serverAddressReader.getNetWorkAddresses();

	// 开启计时器
	timeoutThread = new thread(&State::timeoutCounterThread, this);
	// 开启AppendEntries
	appendEntriesThread = new thread(&State::registerAppendEntries, this);
	// 开启RequestVote
	requestVoteThread = new thread(&State::registerRequestVote, this);
}
State::~State() {
	timeoutThread->join();
	appendEntriesThread->join();
	requestVoteThread->join();
	delete timeoutThread;
	delete appendEntriesThread;
	delete requestVoteThread;
}

void State::timeoutCounterThread() {
	// 超时返回，转换到candidate
	if (timeoutCounter.run())
		nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
			commitIndex, lastApplied, logEntries);
	// 未超时，主动返回，将nextState的初始化留给stop的调用处
	// 将其他的接收线程都停了，这样其他函数可以通过调用停止timeout的函数结束掉其他所有线程
	stopThread();
}

// 注册等待接收AppendEntries
void State::registerAppendEntries() {
	appendEntriesRpcServer.reset(new rpc_server(appendEntriesAddress.second, 6));
	appendEntriesRpcServer->register_handler("appendEntries", [this](rpc_conn conn,
		string appendEntriesCodedIntoString) {
			this->appendEntries(std::move(appendEntriesCodedIntoString));
		});
	//appendEntriesRpcServer->register_handler("appendEntries", appendEntries);
	appendEntriesRpcServer->run();//启动服务端
	cout << "State::registerAppendEntries close AppendEntries" << endl;
}

// 注册投票线程RequestVote
void State::registerRequestVote() {
	requestVoteRpcServer.reset(new rpc_server(requestVoteAddress.second, 6));
	requestVoteRpcServer->register_handler("requestVote", [this](rpc_conn conn,
		string requestVoteCodedIntoString) {
			this->requestVote(std::move(requestVoteCodedIntoString));
		});
	requestVoteRpcServer->run();//启动服务端
	cout << "State::registerRequestVote close RequestVote" << endl;
}

void State::stopThread() {
	requestVoteRpcServer.reset(nullptr);
	appendEntriesRpcServer.reset(nullptr);
}

int State::getCurrentTerm() const {
	return currentTerm;
}
bool State::appendEntriesReal(int prevLogIndex, int prevLogTerm, int leaderCommit, vector<LogEntry> entries) {
	// 如果prevLogIndex比当前entries列表都大，可以直接返回false了
	if (prevLogIndex >= logEntries.size()) return false;
	// 如果prevLogTerm对不上，也可以直接返回false，若prevLogIndex小于0（即-1），则不必检测
	if (prevLogIndex >= 0 && logEntries[prevLogIndex].getTerm() != prevLogTerm) return false;
	// 在对应位置加入entries
	lastApplied = prevLogIndex + 1;
	// 覆写
	for (; lastApplied < logEntries.size() && lastApplied - prevLogIndex - 1 < entries.size(); ++lastApplied) 
		logEntries[lastApplied] = entries[lastApplied - prevLogIndex - 1];
	// 追加
	while (lastApplied - prevLogIndex - 1 < entries.size()) {
		logEntries.push_back(entries[lastApplied - prevLogIndex - 1]);
		lastApplied++;
	}
	// 更新leaderCommit
	commitIndex = leaderCommit;
	// 返回执行成功的信息
	return true;
}