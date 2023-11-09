#include "Command.h"
Command::Command(CommandType commandType, int ID, string key, string value)
	:commandType(commandType), key(key), value(value), ID(ID) {
	
}
bool Command::operator==(Command other)const {
	return ID == other.getID() && key == other.getKey() &&
		value == other.getValue() && commandType == other.getType();
}
