#include "TimeoutCounter.h"
//#include <Windows.h>
TimeoutCounter::TimeoutCounter() {
	receiveInfoFlag = false;
	stop = false;
	std::default_random_engine randomEngine;
	randomEngine.seed(time(0));
	std::uniform_int_distribution<int> u(5, 30); // ×ó±ÕÓÒ±ÕÇø¼ä
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
		// Ë¯ÃßÊ±¼äÆ¬
		sleep_for(seconds(electionTimeouts));
		flagLock.lock();
		if (!receiveInfoFlag) {
			flagLock.unlock();
			return true;
		}
		flagLock.unlock();
	}
	return false;
}
void TimeoutCounter::stopCounter() {
	stop = true;
}