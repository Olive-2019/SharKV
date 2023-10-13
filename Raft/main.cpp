#include "Raft.h"
#include "AppendEntries.h"
#include "RequestVote.h"
int main() {
	string codedstr = "13 output something";
	LogEntry le1(codedstr);
	LogEntry le2(le1.code());
	vector<LogEntry> les({le1, le2});
	AppendEntries appendEntries(0, 1, 2, 3, 4, les);
	string buff = appendEntries.code();
	AppendEntries ae2(buff);
	RequestVote re(0, 1, 2, 3);
	buff = re.code();
	RequestVote re2(buff);
	cout << re2.getLastLogIndex();
	return 0;
}