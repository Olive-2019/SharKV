#pragma once
#include "FileOperator.h"
#include "source.h"
class ServerAddressReader : public FileOperator {
	pair<int, NetWorkAddress> getOneNetWorkAddress();
public:
	ServerAddressReader(string filePath);
	// 获取配置文件中的网络地址
	map<int, NetWorkAddress> getNetWorkAddresses();
};

