#pragma once
#include <include/rest_rpc/rpc_client.hpp>
class Answer
{
public:
	int term;
	bool success;
	MSGPACK_DEFINE(term, success);
};

