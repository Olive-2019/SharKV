#include "Answer.h"
Answer::Answer(int term, bool success) : term(term), success(success) {}
// 反序列化
Answer::Answer(string codedString) {
	int index = 0;
	term = getFirstNumber(codedString, index);
	success = getFirstNumber(codedString, index);
}
// 序列化
string Answer::code() const {
	return to_string(term) + " " + to_string(int(success));
}
int Answer::getTerm() const {
	return term;
}
bool Answer::getSuccess() const {
	return success;
}