#pragma once
#include "RPC.h"
#include "Answer.h"

Answer RPC::invokeRequestVote(NetWorkAddress address, RequestVote requestVote) {
    rpc_client client(address.first, address.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("RPC::invokeRequestVote can't connect");
    
    try {
        Answer ans = client.call<Answer>("requestVote", requestVote);// funcName 为事先注册好的服务名，需要一个 arg 参数
        return ans;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    //cout << "RPC::invokeRemoteFunc " << ans.term << ' ' << ans.success << endl;
}
Answer RPC::invokeAppendEntries(NetWorkAddress address, AppendEntries appendEntries) {
    rpc_client client(address.first, address.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("RPC::invokeAppendEntries can't connect");

    try {
        Answer ans = client.call<Answer>("appendEntries", appendEntries);// funcName 为事先注册好的服务名，需要一个 arg 参数
        return ans;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}

//Answer RPC::invokeRemoteFunc(NetWorkAddress address, string funcName, string arg) {
//    rpc_client client(address.first, address.second);// IP 地址，端口号
//    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
//    bool has_connected = client.connect(5);
//    /*没有建立连接则退出程序*/
//    if (!has_connected) throw exception("RPC::invokeRemoteFunc can't connect");
//
//    /*调用远程服务，返回欢迎信息*/
//    if (funcName == "appendEntries") {
//        cout << "RPC::invokeRemoteFunc appendEntries content:" << arg << endl;
//    }
//    Answer ans;
//    try {
//        auto result = client.call<Answer>(funcName, arg);// funcName 为事先注册好的服务名，需要一个 arg 参数
//        ans = result;
//    }
//    catch (exception e) {
//        cout << e.what() << endl;
//    }
//    //cout << "RPC::invokeRemoteFunc " << ans.term << ' ' << ans.success << endl;
//    return ans;
//}
StartAnswer RPC::invokeRemoteStart(NetWorkAddress address, string arg) {
    rpc_client client(address.first, address.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("RPC::invokeRemoteStart can't connect");

    /*调用远程服务，返回欢迎信息*/
    /*if (funcName == "requestVote") {
        cout << "RPC::invokeRemoteFunc" << endl;
    }*/
    StartAnswer ans;
    try {
        auto result = client.call<StartAnswer>("start", arg);// funcName 为事先注册好的服务名，需要一个 arg 参数
        ans = result;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    //cout << "RPC::invokeRemoteFunc " << ans.term << ' ' << ans.index<< endl;
    return ans;
}
void RPC::invokeRemoteApplyMsg(NetWorkAddress address, string command, int index) {
    rpc_client client(address.first, address.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("RPC::invokeRemoteApplyMsg can't connect");

    /*调用远程服务，返回欢迎信息*/
    /*if (funcName == "requestVote") {
        cout << "RPC::invokeRemoteFunc" << endl;
    }*/
    try {
        client.call<bool>("applyMsg", command, index);
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
}
