#pragma once
#include "source.h"
#include <fstream>
using std::fstream;
class FileOperator
{
	string filePath;
	fstream fileHandle;
public:
	FileOperator(string filePath);
	// 读取每一行，读到末尾会自动关闭文件
	string getOneRaw();
	// 是否读取到文件末尾
	bool isEnd() const;
	// 打开文件，选择模式
	void openFile(std::ios_base::openmode openMode);

};

