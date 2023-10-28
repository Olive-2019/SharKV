#pragma once
#include "State.h"
#include "Follower.h"
#include "Leader.h"
#include "Candidate.h"
#include "PersistenceInfoReaderAndWriter.h"
class Raft
{
	
	State* state;
public:
	
	Raft() : state(NULL){
	}
	// 供client调用，向状态机发送命令，返回值是index和term，以空格间隔
	string start(string command);
	// 完成后向client发送可以执行该命令的请求
	void applyMsg(string command, int index);
	// 运行状态机，调用状态机接口切换状态
	void run(int serverID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		 NetWorkAddress startAddress, NetWorkAddress applyMessageAddress);
};

