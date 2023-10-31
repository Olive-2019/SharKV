#pragma once
#include "POJO.h"
class ApplyMsg : public POJO
{
	string command;
	int index;
	bool snapshot;
public:
	MSGPACK_DEFINE(command, index, snapshot);
	ApplyMsg(string command, int index, bool snapshot) : command(command), index(index), snapshot(snapshot) {}
	string getCommand() const { return command; }
	int getIndex() const { return index; }
	bool isSnapshot() const { return snapshot; }
};

