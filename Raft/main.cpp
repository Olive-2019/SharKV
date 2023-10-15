#include "source.h"
//#include <memory>
#include <thread>
#include "../rest_rpc/include/rest_rpc.hpp"
using namespace rest_rpc::rpc_service;
using std::unique_ptr;
using namespace rest_rpc;
using std::thread;
std::string hello(rpc_conn conn, std::string name) {
	/*可以为 void 返回类型，代表调用后不给远程客户端返回消息*/
	return ("Hello " + name); /*返回给远程客户端的内容*/
}
unique_ptr<rpc_server> p;
void run() {
	p.reset(new rpc_server(9000, 6));
	p->register_handler("func_greet", hello);
	p->run();//启动服务端
	cout << "close" << endl;
}
int main() {
	thread t(run);
	Sleep(500);
	p.reset(nullptr);
	t.join();
	
	return 0;
}