#pragma once
#include "source.h"
#include <fstream>
#include <sstream>
using std::ifstream;
using std::fstream;
using std::stringstream;
class FileOperator
{
protected:

	string filePath;
	fstream fileHandle;
	// 读取每一行，读到末尾会自动关闭文件
	string getOneRaw();
	// 是否读取到文件末尾
	bool isEnd() const;
	// 打开文件，选择模式
	void openFile(std::ios_base::openmode openMode);

	// 检测是否存在文件
	bool isExistFile();
	// append写一行
	void appendOneRow(string content);
	// 新写一行
	void writeOneRow(string content);
public:
	FileOperator(string filePath);
};

