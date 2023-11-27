#include "RPC.h"
void RPC::invokeGetData(NetWorkAddress address, int commandID, string value) {
    rpc_client client(address.first, address.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("RPC::invokeGetData can't connect");

    try {
        client.call<void>("getData", commandID, value);// funcName 为事先注册好的服务名，需要一个 arg 参数
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}
int RPC::invokeRemoteAcceptCommand(NetWorkAddress address, Command command) {
    rpc_client client(address.first, address.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("RPC::invokeRemoteAcceptCommand can't connect");

    try {
        return client.call<int>("acceptCommand", command);// funcName 为事先注册好的服务名，需要一个 arg 参数
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}
// 向KVCtrler申请join
int RPC::invokeJoin(NetWorkAddress address, NetWorkAddress selfAddress) {
    rpc_client client(address.first, address.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("RPC::invokeJoin can't connect");

    try {
        return client.call<int>("join", selfAddress);// funcName 为事先注册好的服务名，需要一个 arg 参数
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}

// 向KVCtrler索要当前group对应的shardid
vector<int> RPC::invokeQueryShardID(NetWorkAddress address, int group) {
    rpc_client client(address.first, address.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("RPC::invokeQueryShardID can't connect");

    try {
        return client.call<vector<int>>("queryShardIDByGroupID", group);// funcName 为事先注册好的服务名，需要一个 arg 参数
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}


// 向KVCtrler索要当前shardid对应的新地址
NetWorkAddress RPC::invokeQueryNewGroup(NetWorkAddress address, int shardID) {
    rpc_client client(address.first, address.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("RPC::invokeQueryNewGroup can't connect");

    try {
        return client.call<NetWorkAddress>("queryGroupAddressByShardID", shardID);// funcName 为事先注册好的服务名，需要一个 arg 参数
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}