#include "State.h"
void State::timeoutCounterThread() {
	timeoutCounter.run();
}