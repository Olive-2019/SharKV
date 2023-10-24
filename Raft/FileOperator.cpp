#include "FileOperator.h"
FileOperator::FileOperator(string filePath) : filePath(filePath) {
}
string FileOperator::getOneRaw() {
	if (!fileHandle.is_open()) throw exception("FileOperator::getOneRaw don't open file");
	string oneRawBuff;
	getline(fileHandle, oneRawBuff);
	if (isEnd()) fileHandle.close();
	return oneRawBuff;
}
bool FileOperator::isEnd() const {
	return !fileHandle.is_open() || fileHandle.eof();
}
void FileOperator::openFile(std::ios_base::openmode openMode) {
	if (fileHandle.is_open()) fileHandle.close();
	fileHandle.open(filePath.c_str(), openMode);
}
// 检测是否存在文件
bool FileOperator::isExistFile() {
	ifstream f(filePath.c_str());
	return f.good();
}
// append写一行
void FileOperator::appendOneRow(string content) {
	openFile(std::ios::app);
	content += "\n";
	fileHandle.write(content.c_str(), content.size());
	fileHandle.close();
}
// 新写一行
void FileOperator::writeOneRow(string content) {
	openFile(std::ios::out);
	content += "\n";
	fileHandle.write(content.c_str(), content.size());
	fileHandle.close();
}