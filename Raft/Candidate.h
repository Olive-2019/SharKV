#pragma once
#include "State.h"
class Candidate : public State
{
	// 投票结果 0:没有收到 1：赢得该选票 -1：输了
	vector<int> voteResult;
public:

};

