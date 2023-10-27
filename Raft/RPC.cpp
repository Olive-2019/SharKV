#include "RPC.h"

Answer RPC::invokeRemoteFunc(NetWorkAddress address, string funcName, string arg) {
    rpc_client client(address.first, address.second);// IP 地址，端口号
    /*设定超时 5s（不填默认为 3s），connect 超时返回 false，成功返回 true*/
    bool has_connected = client.connect(5);
    /*没有建立连接则退出程序*/
    if (!has_connected) throw exception("RPC::invokeRemoteFunc can't connect");

    /*调用远程服务，返回欢迎信息*/
    /*if (funcName == "requestVote") {
        cout << "RPC::invokeRemoteFunc" << endl;
    }*/
    string ans;
    try {
        auto result = client.call<std::string>(funcName, arg);// funcName 为事先注册好的服务名，需要一个 arg 参数
        ans = result;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    cout << "RPC::invokeRemoteFunc " << ans << endl;
    return ans;
}
