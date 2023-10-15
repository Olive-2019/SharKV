#pragma once
#include "POJO.h"

class LogEntry : public POJO
{
	int term;
	string command;
public:
	LogEntry(int term, string command);
	// 序列化
	LogEntry(string codedString);
	// 反序列化
	string code() const;

	string getCommand() const;
	int getTerm() const;
};

