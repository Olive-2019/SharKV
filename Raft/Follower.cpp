#include "Follower.h"
Follower::Follower(int currentTerm, int ID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
	NetWorkAddress startAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries, int votedFor) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, startAddress,
		commitIndex, lastApplied, logEntries, votedFor), leaderID(-1) {
	timeoutThread = NULL;
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
			startAddress, commitIndex, lastApplied, logEntries);
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
void Follower::start(AppendEntries newEntries) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	// 假如有leader，转发给leader，没有就给自己加
	if (serverAddress.find(leaderID) != serverAddress.end())
		rpc.invokeRemoteFunc(serverAddress[leaderID], "start", newEntries.code());
	else State::start(newEntries);
}
// 接收RequestVote
Answer Follower::requestVote(string requestVoteCodedIntoString) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	RequestVote requestVote(requestVoteCodedIntoString);
	if (debug) cout << ID << " receive requestVote Msg from " << requestVote.getCandidateId() << " content is " << requestVote.code() << endl;
	//直接返回false：term < currentTerm
	if (requestVote.getTerm() < currentTerm) return Answer{currentTerm, false};
	currentTerm = requestVote.getTerm();
	//如果 （votedFor == null || votedFor == candidateId） && candidate的log比当前节点新，投票给该节点，否则拒绝该节点
	bool vote = false;
	if ((votedFor < 0 && isNewerThanMe(requestVote.getLastLogIndex(), requestVote.getLastLogTerm()))
		|| votedFor == requestVote.getCandidateId()) {
		votedFor = requestVote.getCandidateId();
		vote = true;
	}
	return Answer{ currentTerm, vote };
}
// 接收AppendEntries
Answer Follower::appendEntries(string appendEntriesCodedIntoString) {
	lock_guard<mutex> lockGuard(receiveInfoLock);
	if (debug) cout << ID << " receive appendEntries Msg" << endl;
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// 超时计时器计数
	timeoutCounter.setReceiveInfoFlag();
	//直接返回false：term < currentTerm or prevLogIndex/Term对应的log不存在
	if ((appendEntries.getTerm() < currentTerm)
		|| appendEntries.getPrevLogIndex() >= logEntries.size()
		|| logEntries[appendEntries.getPrevLogIndex()].getTerm() != appendEntries.getTerm())
		return Answer{ currentTerm, false };
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
	//当leaderCommit > commitIndex时，更新commitIndex = min(leaderCommit, 目前最新entry的index)
	if (appendEntries.getLeaderCommit() > commitIndex) {
		commitIndex = appendEntries.getLeaderCommit();
		if (commitIndex > logEntries.size() - 1) commitIndex = logEntries.size() - 1;
	}
	return Answer{ currentTerm, true };
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
		sleep_for(seconds(2));
	}
}
