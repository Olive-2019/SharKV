#include "Raft.h"
using namespace std::chrono;
using namespace std::this_thread;

Raft::Raft(NetWorkAddress raftServerAddress, int applyMsgPort, KVserver* kvServer) : raftServerAddress(raftServerAddress),
commitedIndex(-1), applyMsgPort(applyMsgPort), debug(false), kvServer(kvServer){
    applyMsgThread = new thread(&Raft::registerApplyMsg, this);
    setDebug();
}
Raft::~Raft() {
    applyMsgThread->join();
    delete applyMsgThread;
}
void Raft::applyMsg(rpc_conn conn, ApplyMsg applyMsg) {
    if (debug) cout << "Raft::applyMsg content:command " << applyMsg.getCommands()[0].getKey() << " index " << applyMsg.getIndex();
    // 更新命令(覆写and加入)
    updateCommands(applyMsg.getCommands());
    // 执行
    execute(applyMsg.getIndex());
    // 若需要写快照，则调用写磁盘的函数
    if (applyMsg.isSnapshot()) snapshot(applyMsg.getIndex());
}
void Raft::registerApplyMsg() {
    rpc_server server(applyMsgPort, 6);
    server.register_handler("applyMsg", &Raft::applyMsg, this);
    server.run();
}

StartAnswer Raft::start(Command command) {
    commands.push_back(command);
    if (debug) cout << "Raft::start command " << command.getID() << " " << command.getKey() << endl;
    rpc_client client(raftServerAddress.first, raftServerAddress.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("Raft::start can't connect");

    /*调用远程服务，返回欢迎信息*/
    StartAnswer ans;
    try {
        StartAnswer ans = client.call<StartAnswer>("start", command);// funcName 为事先注册好的服务名，需要一个 arg 参数
        //if (debug) cout << "Raft::start commnad " << command << " term " << ans.term << " index " << ans.index << endl;
        return ans;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}

void Raft::setDebug() {
    debug = true;
}
void Raft::run() {
    /*vector<Command> commands({ Command(CommandType::Append, "Lam", "55190906"),Command(CommandType::Append, "somebody", "55190922") });
    for (Command command : commands) {
        sleep_for(seconds(3));
        if (debug) cout << "Raft::run " << command.getKey() << endl;
        StartAnswer ans = start(command);
    }*/
}
void Raft::execute(int newCommitIndex) {
    // 执行逻辑
    for (int i = commitedIndex + 1; i <= newCommitIndex; ++i) {
        // 执行命令
        kvServer->execute(commands[i]);
        //if (debug) cout << "Raft::execute " << commands[i].getKey() << endl;
    }
    // 更新committedIndex
    commitedIndex = newCommitIndex;
}

void Raft::snapshot(int snapshotIndex) {
    // 将当前状态写磁盘
    if (debug) cout << "Raft::snapshot write disk" << endl;
    kvServer->snapshot();
    // 修改状态，要加一把大锁
    lock_guard<mutex> lockGuard(stateLock);
    commands.erase(commands.begin(), commands.begin() + snapshotIndex);
    commitedIndex -= (snapshotIndex + 1);
}
void Raft::updateCommands(vector<Command> commands) {
    // 找到第一个相同的command
    int firstIndex = this->commands.size();
    for (int i = 0; i < this->commands.size(); ++i) 
        if (commands[0] == this->commands[i]) {
            firstIndex = i;
            break;
        }
    // 后面就是覆写or追加
    for (int i = 0; i < commands.size(); ++i) {
        if (i + firstIndex >= this->commands.size()) this->commands.push_back(commands[i]);
        else this->commands[i + firstIndex] = commands[i];
    }
    
    //commitedIndex = commands.size() - 1;
}