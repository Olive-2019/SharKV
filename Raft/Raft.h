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
	// 运行状态机，调用状态机接口切换状态
	void run(int serverID, NetWorkAddress appendEntriesAddress, NetWorkAddress requestVoteAddress,
		 NetWorkAddress startAddress, NetWorkAddress applyMessageAddress);
};

