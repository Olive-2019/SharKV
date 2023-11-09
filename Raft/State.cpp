#include "State.h"
#include "Candidate.h"
State::State(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress, 
	NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries,
	int votedFor, int handleNum):
	currentTerm(currentTerm), ID(ID), appendEntriesAddress(appendEntriesAddress), votedFor(votedFor),  handleNum(handleNum), 
	requestVoteAddress(requestVoteAddress), startAddress(startAddress), commitIndex(commitIndex), lastApplied(lastApplied),
	logEntries(logEntries), nextState(NULL), debug(false), applyMessageAddress(applyMessageAddress) {
	setDebug();
	appendEntriesThread = requestVoteThread = startThread = NULL;
}
State::~State() {
	//lock_guard<mutex> lockGuard(receiveInfoLock);
	// 退出这些server线程
	requestVoteRpcServer.reset(nullptr);
	appendEntriesRpcServer.reset(nullptr);
	startRpcServer.reset(nullptr);
	// 将线程join一下
	if (appendEntriesThread) appendEntriesThread->join();
	if (requestVoteThread) requestVoteThread->join();
	if (startThread) startThread->join();
	
	// 释放线程对象
	delete startThread;
	// delete timeoutThread;
	delete appendEntriesThread;
	delete requestVoteThread;

}

bool State::crush(double rate) const {
	return TimeoutCounter().getRandom(0, 100) < 100 * rate;
}

void State::persistence() const {
	PersistenceInfoReaderAndWriter persistenceInfoReaderAndWriter(ID);
	persistenceInfoReaderAndWriter.setCurrentTerm(currentTerm);
	persistenceInfoReaderAndWriter.setEntries(logEntries);
	persistenceInfoReaderAndWriter.setVotedFor(votedFor);
	// 写磁盘动作异步执行，避免影响主线程
	async(&PersistenceInfoReaderAndWriter::write, &persistenceInfoReaderAndWriter);
	//persistenceInfoReaderAndWriter.write();
}

void State::applyMsg(bool snapshot, int snapshotIndex) {
	//if (debug) cout << "Leader::applyMsg content logEntries.size() " << logEntries.size() << " commitIndex " << commitIndex << endl;
	/*
	* snapshot作为写快照标志，若为写快照，则index为快照下标
	* 若为普通applyMsg，则为commitedIndex
	*/
	int applyIndex = commitIndex;
	if (debug) cout << "State::applyMsg applyIndex: " << applyIndex << " snapshot " << snapshot << endl;
	if (snapshot) applyIndex = snapshotIndex;
	if (applyIndex < 0 || applyIndex >= logEntries.size()) throw exception("State::applyMsg logical error: index is negative or greater than the log");
	vector<Command> commands;
	// 发送index包含的所有命令
	for (int i = 0; i <= applyIndex; ++i) commands.push_back(logEntries[i].getCommand());
	// 异步通知上层应用写快照/执行命令（非可信交互）
	async(&RPC::invokeRemoteApplyMsg, &rpc, applyMessageAddress, ApplyMsg(commands, applyIndex, snapshot));
	//rpc.invokeRemoteApplyMsg(applyMessageAddress, ApplyMsg(commands, applyIndex, snapshot));
}
void State::snapShotModifyState(int snapshotIndex) {
	// 通知上层应用快照写磁盘
	applyMsg(true, snapshotIndex);
	// 删除命令
	logEntries.erase(logEntries.begin(), logEntries.begin() + snapshotIndex);
	// 修改commit状态
	commitIndex -= (snapshotIndex + 1);
}


void State::printState() {
	cout << endl << endl;
	cout << "There are " << logEntries.size() << " log entries in this state." << endl;
	for (LogEntry entry : logEntries) {
		cout << "term: " << entry.getTerm() << " command: " << entry.getCommand().getType() << endl;
	}
	cout << endl << endl;
}


// 注册等待接收AppendEntries
void State::registerAppendEntries() {
	appendEntriesRpcServer.reset(new rpc_server(appendEntriesAddress.second, handleNum));
	registerHandleAppendEntries();
	appendEntriesRpcServer->run();//启动服务端
	if (debug) cout << "State::registerAppendEntries close AppendEntries" << endl;
}

// 注册投票线程RequestVote
void State::registerRequestVote() {
	requestVoteRpcServer.reset(new rpc_server(requestVoteAddress.second, handleNum));
	registerHandleRequestVote();
	requestVoteRpcServer->run();//启动服务端
	if (debug) cout << "State::registerRequestVote close RequestVote" << endl;
}

void State::registerServer() {
	// 开启接收start的线程
	startThread = new thread(&State::registerStart, this);
	// 开启AppendEntries
	appendEntriesThread = new thread(&State::registerAppendEntries, this);
	// 开启RequestVote
	requestVoteThread = new thread(&State::registerRequestVote, this);
}

// 注册start函数
void State::registerStart() {
	startRpcServer.reset(nullptr);
	startRpcServer.reset(new rpc_server(startAddress.second, handleNum));
	registerHandleStart();
	startRpcServer->run();//启动服务端
	if (debug) cout << "State::registerStart close start" << endl;
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

StartAnswer State::start(rpc_conn conn, Command command) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (debug) cout << ID << " receive the command from client, content: " << command.getKey() << endl;
	//将client给的数据加入当前列表中
	logEntries.push_back(LogEntry(currentTerm, command));
	// 有新增加的entries，更新lastApplied
	lastApplied = logEntries.size() - 1;
	return StartAnswer{ currentTerm, lastApplied };
}
State* State::run() {
	registerServer();
	work();
	
	return nextState;
}