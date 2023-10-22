#include "TimeoutCounter.h"
#include <Windows.h>
TimeoutCounter::TimeoutCounter() {
	receiveInfoFlag = false;
	stop = false;
	std::default_random_engine randomEngine;
	randomEngine.seed(time(0));
	std::uniform_int_distribution<int> u(150, 300); // ×ó±ÕÓÒ±ÕÇø¼ä
	electionTimeouts = u(randomEngine);
}
void TimeoutCounter::setReceiveInfoFlag() {
	flagLock.lock();
	receiveInfoFlag = true;
	flagLock.unlock();
}
bool TimeoutCounter::run() {
	while (!stop) {
		flagLock.lock();
		receiveInfoFlag = false;
		flagLock.unlock();
		Sleep(electionTimeouts);
		flagLock.lock();
		if (!receiveInfoFlag) return true;
		flagLock.unlock();
	}
	return false;
}
void TimeoutCounter::stopCounter() {
	stop = true;
}