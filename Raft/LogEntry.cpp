#include "LogEntry.h"
LogEntry::LogEntry(int term, Command command) : term(term), command(command){}
LogEntry::LogEntry(string codedString) {
	term = 0;
	int index = 0;
	term = getFirstNumber(codedString, index);
	/*while (codedString[index] >= '0' && codedString[index] <= '9') {
		term *= 10;
		term += (codedString[index++] - '0');
	}*/
	int ID = getFirstNumber(codedString, index);
	string key = getNextString(codedString, index), value = getNextString(codedString, index);
	CommandType commandType = CommandType(getFirstNumber(codedString, index));
	NetWorkAddress clerkAddress;
	clerkAddress.first = getNextString(codedString, index);
	clerkAddress.second = getFirstNumber(codedString, index);
	command = Command(commandType, clerkAddress, key, value);
	command.setID(ID);
}
string LogEntry::code() const {
	string buff = to_string(term) + " " + to_string(command.getID()) + " " + command.getKey()
		+ " " + command.getValue() + " " + to_string(command.getType()) + " " + command.getClerkAddress().first 
		+ " " + to_string(command.getClerkAddress().second);
	return buff;
}

Command LogEntry::getCommand() const {
	return command;
}
int LogEntry::getTerm() const {
	return term;
}