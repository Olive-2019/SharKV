#pragma once
#include "source.h"
enum CommandType
{
	Append, Put, Get
};
class Command
{
	// 当前命令的类型
	CommandType commandType;
	// 命令的参数
	string key, value;
public:
	Command(CommandType commandType, string key = "", string value = "");
	CommandType getType() const { return commandType; }
	string getKey() const { return key; }
	string getValue() const { return value; }

};

