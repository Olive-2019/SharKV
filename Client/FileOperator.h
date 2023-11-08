#pragma once
#include "source.h"
#include <boost/serialization/map.hpp> 
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include <sstream>
#include <fstream>
using std::fstream;
using std::ostringstream;
class FileOperator
{
	string filePath;
public:
	FileOperator(string filePath);
	// 将map序列化并向filePath写入
	void write(const map<string, string>& data);
	// 从file中读出数据并反序列化为map<string, string>
	map<string, string> read()const;
};

