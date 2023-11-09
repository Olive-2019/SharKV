#pragma once
#include "POJO.h"
#include "Command.h"
class LogEntry : public POJO
{
	int term;
	Command command;
public:
	MSGPACK_DEFINE(term, command);
	LogEntry() {}
	LogEntry(int term, Command command);
	//// 序列化
	//LogEntry(string codedString);
	//// 反序列化
	////MSGPACK_DEFINE(term, command);
	//string code() const;

	Command getCommand() const;
	int getTerm() const;
};

