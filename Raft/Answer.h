#pragma once
#include "POJO.h"
#include <include/rest_rpc/rpc_client.hpp>
class Answer: public POJO
{
	int term;
	bool success;
public:
	MSGPACK_DEFINE(term, success);
	Answer(){}
	Answer(int term, bool success):term(term), success(success){}
	int getTerm() const { return term; };
	bool isSuccess() const { return success; };
};

