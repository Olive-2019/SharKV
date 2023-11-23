#pragma once
#include "source.h"
#include "Command.h"
class RPC
{
public:
	// 向Clerk传输数据
	void invokeGetData(NetWorkAddress address, int commandID, string value);
	// 向KVCtrler申请join
	int invokeJoin(NetWorkAddress address, NetWorkAddress selfAddress);
};

