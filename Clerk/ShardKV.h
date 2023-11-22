#pragma once
#include "source.h"
#include "Clerk.h"
class ShardKV
{
	Clerk clerk;
	NetWorkAddress queryGroupAddressByShardIDAddress, queryShardIDByGroupIDAddress;
	RPC rpc;
	set<int> shardIDs;
public:
	ShardKV(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress,
		NetWorkAddress queryGroupAddressByShardIDAddress, NetWorkAddress queryShardIDByGroupIDAddress);

	

};

