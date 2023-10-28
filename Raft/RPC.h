#pragma once
#include "source.h"
#include "Answer.h"
#include "../rest_rpc/include/rest_rpc.hpp"
using namespace rest_rpc::rpc_service;
using namespace rest_rpc;
//新增调用RPC的类/
class RPC
{

public:
	// 调用函数（阻塞）
	Answer invokeRemoteFunc(NetWorkAddress address, string funcName, string arg);
	
};

