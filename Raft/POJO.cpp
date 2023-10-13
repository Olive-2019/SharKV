#include "POJO.h"
int POJO::getFirstNumber(const string& buff, int& index) const {
	int number = 0;
	while (buff[index] >= '0' && buff[index] <= '9') {
		number *= 10;
		number += (buff[index++] - '0');
	}
	index++;
	return number;
}

string POJO::getSettedLenString(const string& buff, int& index, int size) const {
	if (index + size > buff.size()) throw exception("POJO::getSettedLenString: index is too large to read");
	string newString = buff.substr(index, size);
	index += size;
	return newString;
}