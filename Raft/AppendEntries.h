#pragma once
#include "LogEntry.h"
#include <sstream>
#include "POJO.h"
#include "LogEntry.h"
#include <include/rest_rpc/rpc_client.hpp>
#include <vector>
using std::vector;
using std::stringstream;
class AppendEntries : public POJO
{
	//leader的当前term编号
	int term;
	// leader的id，方便follower将收到的信息转发给leader
	int leaderId;
	// 前一个log的index，方便follower确认一致性
	int prevLogIndex;
	// 前一个log的term，功能同上
	int prevLogTerm;
	// leader提交的日志 entry index
	int leaderCommit;
	// 需要拷贝的多条log entry，心跳信息会是空
	vector<LogEntry> entries;
	//MSGPACK_DEFINE(term, leaderId, prevLogIndex);
public:
	AppendEntries() {  }
	AppendEntries(int term, int leaderId, int prevLogIndex, int prevLogTerm, int leaderCommit, vector<LogEntry> entries);
	// 反序列化
	AppendEntries(string codedString);
	// 序列化
	string code() const;
	MSGPACK_DEFINE(term, leaderId, prevLogIndex, prevLogTerm, leaderCommit, entries);
	int getTerm()const { return term; }
	int getLeaderId()const { return leaderId; }
	int getPrevLogIndex()const { return prevLogIndex; }
	int getPrevLogTerm()const { return prevLogTerm; }
	int getLeaderCommit()const { return leaderCommit; }
	vector<LogEntry> getEntries()const { return entries; }
};

