#include "Follower.h"
Follower::Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, NetWorkAddress applyMessageAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress, applyMessageAddress,
		commitIndex, lastApplied, logEntries, votedFor), leaderID(-1), timeoutThread(NULL) {
	if (debug) cout << "Follower::Follower new a Follower" << endl;

}
Follower::~Follower() {
	timeoutCounter.stopCounter();
	if (timeoutThread) timeoutThread->join();
	delete timeoutThread;
	if (debug) cout << ID << " will not be Follower any more." << endl;
}
void Follower::timeoutCounterThread() {
	// 超时返回，转换到candidate
	if (timeoutCounter.run()) {
		nextState = new Candidate(currentTerm + 1, ID, appendEntriesAddress, requestVoteAddress,
			startAddress, applyMessageAddress, commitIndex, lastApplied, logEntries);
		if (debug) cout << "Follower " << ID << " timeout and quit." << endl;
	}
	else if (debug) cout << "Follower " << ID << " quit." << endl;
	
	// 未超时，主动返回，将nextState的初始化留给stop的调用处
}
bool Follower::isNewerThanMe(int lastLogIndex, int lastLogTerm) const {
	if (!logEntries.size()) return true;
	if (logEntries.back().getTerm() == currentTerm) return logEntries.size() < lastLogIndex + 1;
	return currentTerm < logEntries.back().getTerm();
}
StartAnswer Follower::start(rpc_conn conn, string command) {
	// 不能在这加锁，否则其他线程都要等着转发start的返回值
	//lock_guard<mutex> lockGuard(receiveInfoLock);
	// 假如有leader，转发给leader，没有就给自己加
	if (serverAddress.find(leaderID) != serverAddress.end())
		return rpc.invokeRemoteStart(serverAddress[leaderID], command);
	
	lock_guard<mutex> lockGuard(receiveInfoLock);
	//将client给的数据加入当前列表中
	logEntries.push_back(LogEntry(currentTerm, command));
	// 有新增加的entries，更新lastApplied
	lastApplied = logEntries.size() - 1;
	return StartAnswer{ currentTerm, lastApplied };
	
}
// 接收RequestVote
Answer Follower::requestVote(rpc_conn conn, RequestVote requestVote) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	//RequestVote requestVote(requestVoteCodedIntoString);
	//if (debug) cout << ID << " receive requestVote Msg from " << requestVote.getCandidateId() << " content is " << requestVote.code() << endl;
	//直接返回false：term < currentTerm
	if (requestVote.getTerm() < currentTerm) return Answer(currentTerm, false);
	currentTerm = requestVote.getTerm();
	//如果 （votedFor == null || votedFor == candidateId） && candidate的log比当前节点新，投票给该节点，否则拒绝该节点
	bool vote = false;
	if ((votedFor < 0 && isNewerThanMe(requestVote.getLastLogIndex(), requestVote.getLastLogTerm()))
		|| votedFor == requestVote.getCandidateId()) {
		votedFor = requestVote.getCandidateId();
		vote = true;
	}
	//if (debug) cout << "Follower::requestVote: send to " << requestVote.getCandidateId()
		//<< ", content is " << currentTerm << ' ' << vote << endl;
	return Answer(currentTerm, vote);
}
// 接收AppendEntries
Answer Follower::appendEntries(rpc_conn conn, AppendEntries appendEntries) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	//AppendEntries appendEntries(appendEntriesCodedIntoString);
	//if (debug) cout << "Follower::appendEntries : content " << appendEntriesCodedIntoString << endl;
	if (debug) cout << "Follower::appendEntries from " << appendEntries.getLeaderId() <<  " : log entries size: " << appendEntries.getEntries().size();
	// 超时计时器计数
	timeoutCounter.setReceiveInfoFlag();
	//直接返回false：term < currentTerm 
	if (appendEntries.getTerm() < currentTerm) return Answer(currentTerm, false);
	// 心跳返回true
	if (!appendEntries.getEntries().size()) return Answer(currentTerm, true);
	//直接返回false：prevLogIndex/Term对应的log不存在
	if (appendEntries.getPrevLogIndex() >= 0 && (appendEntries.getPrevLogIndex() >= logEntries.size()
		|| logEntries[appendEntries.getPrevLogIndex()].getTerm() != appendEntries.getTerm()))
		return Answer(currentTerm, false);
	currentTerm = appendEntries.getTerm();
		
	int index = appendEntries.getPrevLogIndex() + 1;
	// 更新leaderID
	leaderID = appendEntries.getLeaderId();
	for (LogEntry entry : appendEntries.getEntries()) {
		//存在冲突entry，即相同的index，不同的term（就是leader没有的数据），删掉现有的entries并写入leader给的数据
		if (index < logEntries.size()) logEntries[index] = entry;
		//将不存在的entries追加到系统中
		else logEntries.push_back(entry);
		index++;
	}
	commitIndex = appendEntries.getLeaderCommit();
	////当leaderCommit > commitIndex时，更新commitIndex = min(leaderCommit, 目前最新entry的index)
	//if (appendEntries.getLeaderCommit() > commitIndex) {
	//	commitIndex = appendEntries.getLeaderCommit();
	//	if (commitIndex > logEntries.size() - 1) commitIndex = logEntries.size() - 1;
	//}
	if (debug) cout << "Follower::appendEntries real " << appendEntries.getEntries()[0].getCommand() << endl;

	return Answer( currentTerm, true );
}
void Follower::work() {


	if (debug) cout << endl << ID << " become Follower" << endl;
	// 读入集群中所有server的地址，follower读入StartAddress的地址
	ServerAddressReader serverAddressReader("StartAddress.conf");
	serverAddress = serverAddressReader.getNetWorkAddresses();
	// 开启计时器
	timeoutThread = new thread(&Follower::timeoutCounterThread, this);
	// 用nextState作为同步信号量,超时/收到更新的信息的时候就可以退出了
	while (!nextState) {
		// 睡眠一段时间
		if (debug) printState();
		sleep_for(seconds(3));
		persistence();
		// 模拟停机
		/*if (crush(0.1)) {
			if (debug) cout << "crush" << endl;
			break;
		}*/
	}
}



// 注册等待接收AppendEntries句柄
void Follower::registerHandleAppendEntries() {
	appendEntriesRpcServer->register_handler("appendEntries", &Follower::appendEntries, this);
}
// 注册投票线程RequestVote句柄
void Follower::registerHandleRequestVote() {
	requestVoteRpcServer->register_handler("requestVote", &Follower::requestVote, this);
}
// 注册start函数句柄
void Follower::registerHandleStart() {
	startRpcServer->register_handler("start", &Follower::start, this);
}