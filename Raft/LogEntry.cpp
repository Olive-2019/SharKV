#include "LogEntry.h"
LogEntry::LogEntry(int term, string command) : term(term), command(command){}
LogEntry::LogEntry(string codedString) {
	term = 0;
	int index = 0;
	while (codedString[index] >= '0' && codedString[index] <= '9') {
		term *= 10;
		term += (codedString[index++] - '0');
	}
	command = codedString.substr(index + 1);
}
string LogEntry::code() const {
	string buff = to_string(term) + " " + command;
	return buff;
}

string LogEntry::getCommand() const {
	return command;
}
int LogEntry::getTerm() const {
	return term;
}