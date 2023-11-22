#include "ShardKV.h"
ShardKV::ShardKV(NetWorkAddress kvServerAddress, NetWorkAddress getDataAddress,
	NetWorkAddress queryGroupAddressByShardIDAddress, NetWorkAddress queryShardIDByGroupIDAddress)
	: clerk(kvServerAddress, getDataAddress), queryGroupAddressByShardIDAddress(queryGroupAddressByShardIDAddress),
	queryShardIDByGroupIDAddress(queryShardIDByGroupIDAddress) {

}