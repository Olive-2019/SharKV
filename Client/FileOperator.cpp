#include "FileOperator.h"

FileOperator::FileOperator(string filePath) : filePath(filePath){}


// 将map序列化并向filePath写入
void FileOperator::write(const map<string, string>& data) {
	ostringstream temp;
	boost::archive::text_oarchive oa(temp);
	oa << data;
	string s = temp.str();
	fstream outputHandle(filePath, std::ios_base::out);
	if (!outputHandle.good()) throw exception("FileOperator::write can't open file");
	outputHandle.write(s.c_str(), s.size() + 1);
	outputHandle.close();
}
// 从file中读出数据并反序列化为map<string, string>
map<string, string> FileOperator::read()const {
	map<string, string> data;
	fstream inputHandle(filePath, std::ios_base::in);
	if (!inputHandle.good()) throw exception("FileOperator::read can't open file");
	string rawData((std::istreambuf_iterator<char>(inputHandle)),
		std::istreambuf_iterator<char>());
	inputHandle.close();
	std::istringstream iss(rawData);//通过构造函数对istringstream类进行赋值，可以将一个字符串变量的值传递给
	boost::archive::text_iarchive ia(iss);
	ia >> data;
	return data;
}
