#include "Leader.h"
#include "Follower.h"
Leader::Leader(int currentTerm, int ID, NetWorkAddress appendEntriesAddress,
	NetWorkAddress requestVoteAddress, int commitIndex, int lastApplied, vector<LogEntry> logEntries) :
	State(currentTerm, ID, appendEntriesAddress, requestVoteAddress, commitIndex, lastApplied, logEntries),
	nextIndex(serverAddress.size(), logEntries.size()), matchIndex(serverAddress.size(), 0)
{
	for ()
	// 发送心跳
}
// 接收RequestVote
string Leader::requestVote(string requestVoteCodedIntoString) {
	receiveInfoLock.lock();
	
	RequestVote requestVote(requestVoteCodedIntoString);
	// term没有比当前leader大，可以直接拒绝，并返回当前的term
	if (requestVote.getTerm() <= currentTerm) {
		receiveInfoLock.unlock();
		return Answer(currentTerm, false).code();
	}
	// term更新，则退出当前状态，返回到Follower的状态
	currentTerm = requestVote.getTerm();
	
	// 理应在返回结果以后结束掉接收线程，但是此处无法这么处理
	// 所以用nextState作为信号量，保证线程间同步释放
	if (nextState && nextState->getCurrentTerm() < currentTerm) delete nextState;
	// 生成下一状态机
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		commitIndex, lastApplied, logEntries);
	receiveInfoLock.unlock();
	return Answer(currentTerm, true).code();
}
// 接收AppendEntries
string Leader::appendEntries(string appendEntriesCodedIntoString) {
	receiveInfoLock.lock();
	AppendEntries appendEntries(appendEntriesCodedIntoString);
	// timeoutCounter.setReceiveInfoFlag();
	// term没有比当前leader大，可以直接拒绝，并返回当前的term
	if (appendEntries.getTerm() <= currentTerm) {
		receiveInfoLock.unlock();
		return Answer(currentTerm, false).code();
	}
	// term更新，则退出当前状态，返回到Follower的状态
	currentTerm = appendEntries.getTerm();
	// 将entries添加到当前列表中（调用函数，还需要判断其能否添加，这一步其实已经算是follower的工作了）
	bool canAppend = appendEntriesReal(appendEntries.getPrevLogIndex(), appendEntries.getPrevLogTerm(),
		appendEntries.getLeaderCommit(), appendEntries.getEntries());
	// 生成下一状态机
	nextState = new Follower(currentTerm, ID, appendEntriesAddress, requestVoteAddress,
		commitIndex, lastApplied, logEntries);
	receiveInfoLock.unlock();
	return Answer(currentTerm, canAppend).code();
}
State* Leader::run() {
	State::run();
	work();
	waitThread();
	return nextState;
}

void Leader::start(AppendEntries newEntries) {
	receiveInfoLock.lock();
	//将client给的数据加入当前列表中
	merge(logEntries.begin(), logEntries.end(), newEntries.getEntries().begin(),
		newEntries.getEntries().end(), std::back_inserter(logEntries));
	// 有新增加的entries，更新lastApplied
	lastApplied += newEntries.getEntries().size();
	receiveInfoLock.unlock();
}
// 注册start函数
void Leader::registerStart() {
	startRpcServer.reset(nullptr);
	startRpcServer.reset(new rpc_server(startAddress.second, 6));
	startRpcServer->register_handler("start", [this](rpc_conn conn,
		string newEntries) {
			this->start(std::move(newEntries));
		});
	startRpcServer->run();//启动服务端
	cout << "Leader::registerStart close start" << endl;
}

//给其他所有进程同步log entries
void Leader::work() {
	// 用nextState作为同步信号量
	while (!nextState) {
		/*初始化next*/
	}
}
Answer Leader::sendAppendEntries(int followerID, int start, int end) {
	if (end >= logEntries.size()) throw exception("Leader::sendAppendEntries end is illegal");
	vector<LogEntry> entries;
	// 发送的index和term先都初始化为-2，若真的有需要发送的内容再赋值 
	int prevIndex = -1, prevTerm = -1;
	if (start >= 0) {
		prevIndex = start - 1;
		// 若前一个存在，即现在发的不是第一条
		if (prevIndex >= 0) prevTerm = logEntries[prevIndex].getTerm();
		for (int i = start; i <= end; ++i) entries.push_back(logEntries[i]);
	}
	// 获取可以传输的字符串
	string appendEntriesStr = AppendEntries(currentTerm, ID, prevIndex, prevTerm, commitIndex, entries).code();
	rpc_client client(serverAddress[followerID].first, serverAddress[followerID].second);// IP 地址，端口号
	/*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
	bool has_connected = client.connect(5);
	/*没有建立连接则退出程序*/
	if (!has_connected) throw exception("Leader::sendAppendEntries connect timeout");
	string appendEntriesRes = client.call<string>("appendEntries", appendEntriesStr);
	return Answer(appendEntriesRes);
}