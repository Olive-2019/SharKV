#pragma once
#include "State.h"
class Leader : public State
{
	// 需要发给每一个follower的下一条log entry（初始化值是leader的最后一个log entry的下一个值）
	vector<int> nextIndex;
	// 每个follower当前匹配到哪一条log entry（初始化值为0）
	vector<int> matchIndex;
};

