#include "RequestVote.h"
RequestVote::RequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm) : 
	term(term), candidateId(candidateId), lastLogIndex(lastLogIndex), lastLogTerm(lastLogTerm) {}

//RequestVote::RequestVote(string codedString) {
//	int index = 0;
//	term = getFirstNumber(codedString, index);
//	candidateId = getFirstNumber(codedString, index);
//	lastLogIndex = getFirstNumber(codedString, index);
//	lastLogTerm = getFirstNumber(codedString, index);
//}

string RequestVote::code() const {
	string buff = to_string(term) + " " + to_string(candidateId) + " " + 
		to_string(lastLogIndex) + " " + to_string(lastLogTerm);
	return buff;
}