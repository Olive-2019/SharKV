#pragma once
#include "source.h"
#include <include/rest_rpc/rpc_client.hpp>
class POJO
{
protected:
	/*编码工具*/
	//返回第一个读到的数字，index会增加,并跳过间隔空格
	int getFirstNumber(const string& buff, int& index) const;
	//返回指定长度字符串，index会增加
	string getSettedLenString(const string& buff, int& index, int size)const;
	// 返回从指定位置开始到下一个空格的字符串
	string getNextString(const string& buff, int& index) const;
public:
};

