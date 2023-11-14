#include "Command.h"
Command::Command(CommandType commandType, string key, string value)
	:commandType(commandType), key(key), value(value) {
	/*lock_guard<mutex> lockGuard(IDCounterLock);
	ID = IDCounter++;*/
}
//mutex Command::IDCounterLock;
//int Command::IDCounter = 0;
bool Command::operator==(Command other)const {
	return ID == other.getID() && key == other.getKey() &&
		value == other.getValue() && commandType == other.getType();
}
