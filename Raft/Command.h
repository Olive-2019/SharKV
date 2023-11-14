#pragma once
#include "source.h"
#include <mutex>
using std::mutex;
enum CommandType
{
	Append, Put, Get
};
class Command
{
	// 发送命令的clerk地址
	NetWorkAddress clerkAddress;
	// 当前命令的类型
	int commandType;
	// 命令的参数
	string key, value;
	// 命令的ID：非唯一标识符
	int ID;
public:
	MSGPACK_DEFINE(commandType, key, value, ID, clerkAddress);
	Command(){}
	Command(CommandType commandType, int ID, string key, string value = "");
	bool operator==(Command other)const;
	int getID() const { return ID; }
	CommandType getType() const { return CommandType(commandType); }
	string getKey() const { return key; }
	string getValue() const { return value; }

};

