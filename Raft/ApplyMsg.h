#pragma once
#include "POJO.h"
#include "Command.h"
class ApplyMsg : public POJO
{
	vector<Command> commands;
	int index;
	bool snapshot;
public:
	MSGPACK_DEFINE(commands, index, snapshot);
	ApplyMsg(vector<Command> commands, int index, bool snapshot) : commands(commands), index(index), snapshot(snapshot) {}
	vector<Command> getCommands() const { return commands; }
	int getIndex() const { return index; }
	bool isSnapshot() const { return snapshot; }
};

