# 模块划分与对应类
整个系统分为五个模块，分别是通信模块、外观模块、计时模块、状态机模块、持久化模块以及通信单元POJO模块。
通信模块封装RPC调用功能，外观模块封装调用状态机及其转换的功能，计时模块负责管理超时逻辑，状态机模块是整个系统的核心，不同的状态机负责不同的逻辑处理，持久化模块负责持久化功能，通信单元POJO模块将RPC内容封装成一个个POJO。
```mermaid
classDiagram
    class Raft{
        Raft
    }
    note for Raft  "外观模块\n封装调用状态机及其转换的功能"
    class State{
        State
        Leader
        Follower
        Candidate
    }
    note for State  "状态机模块\n系统的核心，\n不同的状态机负责不同的逻辑处理"
    class ORM{
        FileOperator
        PersistenceInfoReaderAndWriter
        ServerAddressReader
    }
    note for ORM  "持久化模块\n数据持久化功能"
    class RPC{
        RPC
    }
    note for RPC  "通信模块\n封装RPC调用功能"
    class Timeout{
        TimeoutCounter
    }
    note for Timeout  "计时模块\n管理超时逻辑"
    class POJO{
        POJO
        RequestVote
        AppendEntries
        Answer
        LogEntry
        StartAnswer
        ApplyMsg
    }
    note for POJO  "通信单元模块\n将RPC内容封装成一个个POJO"

```

# 相关线程
## Leader
Leader的任务包括
1. 接收client端/其他follower转发的命令（start）
2. 回应client端（applyMsg）
3. 发送appendEntries
4. 接收appendEntries
5. 接收RequestVote
6. 根据现有命令数量发起snapshot

因此，Leader的线程会包括：

## Follower
## Candidate
# 核心类的接口
