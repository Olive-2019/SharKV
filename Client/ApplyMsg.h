#pragma once
#include "POJO.h"
class ApplyMsg : public POJO
{
	vector<string> commands;
	int index;
	bool snapshot;
public:
	MSGPACK_DEFINE(commands, index, snapshot);
	ApplyMsg(vector<string> commands, int index, bool snapshot) : commands(commands), index(index), snapshot(snapshot) {}
	vector<string> getCommands() const { return commands; }
	int getIndex() const { return index; }
	bool isSnapshot() const { return snapshot; }
};

