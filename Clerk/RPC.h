#pragma once
#include "source.h"
#include "Command.h"
class RPC
{
public:
	void invokeRemoteAcceptCommand(NetWorkAddress address, Command command);
};

