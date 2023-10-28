#pragma once
#include "source.h"
#include <include/rest_rpc/rpc_client.hpp>
class StartAnswer
{
public:
	int term;
	int index;
	MSGPACK_DEFINE(term, index);
};

