#pragma once
#include <random>
#include <mutex>
using std::mutex;
class TimeoutCounter
{
	int electionTimeouts;
	//flag用于标志该周期内是否收到心跳，如果收到则置true
	bool receiveInfoFlag;
	//用于防止flag幻读
	mutex flagLock;
public:
	// 初始化超时计时器，随机一个超时阈值
	TimeoutCounter();
	// 收到信息时设置标志位
	void setReceiveInfoFlag();
	// 阻塞运行，如果能返回，说明这个周期超时了
	void run();
};

