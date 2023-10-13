#include "AppendEntries.h"
AppendEntries::AppendEntries(int term, int leaderId, int prevLogIndex, 
	int prevLogTerm, int leaderCommit, vector<LogEntry> entries) : 
term(term), leaderId(leaderId), prevLogIndex(prevLogIndex), 
prevLogTerm(prevLogTerm), entries(entries), leaderCommit(leaderCommit){}
AppendEntries::AppendEntries(string codedString) {
	// 编码规则：前面的数是按顺序的属性，最后的vector以“n (len content) * n”的形式给出
	int index = 0, n, len;
	term = getFirstNumber(codedString, index);
	prevLogIndex = getFirstNumber(codedString, index);
	prevLogTerm = getFirstNumber(codedString, index);
	leaderCommit = getFirstNumber(codedString, index);
	leaderId = getFirstNumber(codedString, index);
	n = getFirstNumber(codedString, index);
	while (n--) {
		len = getFirstNumber(codedString, index);
		string logEntryString = getSettedLenString(codedString, index, len);
		entries.push_back(LogEntry(logEntryString));
	}
}
string AppendEntries::code() const {
	int n = entries.size();
	string buff = to_string(term) + " " + to_string(prevLogIndex) + " " + to_string(prevLogTerm) + " "
		+ to_string(leaderCommit) + " " + to_string(leaderId) + " " + to_string(n) + " ";
	for (int i = 0; i < n; ++i) {
		string entryBuff = entries[i].code();
		buff += to_string(entryBuff.size());
		buff += " ";
		buff += entryBuff;
	}
	return buff;
}