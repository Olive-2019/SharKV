#pragma once
#include "POJO.h"
#include <include/rest_rpc/rpc_client.hpp>

class LogEntry : public POJO
{
	int term;
	string command;
public:
	LogEntry() {}
	LogEntry(int term, string command);
	// 序列化
	LogEntry(string codedString);
	// 反序列化
	MSGPACK_DEFINE(term, command);
	string code() const;

	string getCommand() const;
	int getTerm() const;
};

