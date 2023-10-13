#pragma once
#include <string>
using std::string;
using std::to_string;
class LogEntry
{
	int term;
	string command;
public:
	LogEntry(int term, string command);
	LogEntry(string codedString);
	string code() const;
	string getCommand() const;
	int getTerm() const;
};

