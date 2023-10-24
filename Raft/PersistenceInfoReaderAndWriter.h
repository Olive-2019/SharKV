#pragma once
#include "FileOperator.h"
#include "LogEntry.h"
class PersistenceInfoReaderAndWriter :
    public FileOperator {
    int currentTerm;
    int votedFor;
    vector<LogEntry> entries;
public:
    PersistenceInfoReaderAndWriter(int serverID);

    // 设置数据
    void setCurrentTerm(int currentTerm);
    void setVotedFor(int votedFor);
    void setEntries(vector<LogEntry> entries);

    // 读取数据
    int getCurrentTerm() const;
    int getVotedFor() const;
    vector<LogEntry> getEntries() const;

    // 从磁盘中读出数据
    bool read();
    // 向磁盘写数据
    void write();

};

