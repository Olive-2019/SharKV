#pragma once
#include "source.h"
#include "Command.h"
class RPC
{
public:
	int invokeRemoteAcceptCommand(NetWorkAddress address, Command command);
};

