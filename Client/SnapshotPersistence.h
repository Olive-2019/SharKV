#pragma once
#include "FileOperator.h"
class SnapshotPersistence :
    public FileOperator
{
    const string snapshotFilePath;
public:
    SnapshotPersistence(string snapshotFilePath):snapshotFilePath(snapshotFilePath), FileOperator(snapshotFilePath) {}
};

