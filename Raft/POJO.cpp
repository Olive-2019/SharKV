#include "POJO.h"
int POJO::getFirstNumber(const string& buff, int& index) const {
	int number = 0;
	while (index < buff.size() && buff[index] >= '0' && buff[index] <= '9') {
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

// 返回从指定位置开始到下一个空格的字符串
string POJO::getNextString(const string& buff, int& index) const {
	int len = 0;
	while (index + len < buff.size() && buff[index + len] != ' ') len++;
	return getSettedLenString(buff, index, len);
}