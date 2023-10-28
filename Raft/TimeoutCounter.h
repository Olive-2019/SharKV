#pragma once
#include <random>
#include <mutex>
#include <thread>
#include <chrono>
using std::this_thread::sleep_for;
using std::chrono::seconds;

using std::mutex;
class TimeoutCounter
{
	int electionTimeouts;
	// flag用于标志该周期内是否收到心跳，如果收到则置true
	bool receiveInfoFlag;
	// 用于防止flag幻读
	mutex flagLock;
	// 是否停止
	bool stop;
public:
	// 初始化超时计时器，随机一个超时阈值
	TimeoutCounter();
	// 返回一个随机数，分布采用平均分布
	int getRandom(int start, int end);
	// 收到信息时设置标志位
	void setReceiveInfoFlag();
	// 阻塞运行
	// 返回值为true时说明超时返回，为false说明是被系统主动暂停的（被stop了）
	bool run();
	// 计时器停止，退出
	void stopCounter();
};

