#pragma once
#include "POJO.h"
class Answer : public POJO
{
	int term;
	bool success;
public:
	Answer(int term, bool success);
	// 反序列化
	Answer(string codedString);
	// 序列化
	string code() const;

	int getTerm() const;
	bool getSuccess() const;
};

