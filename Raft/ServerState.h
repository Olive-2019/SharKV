#pragma once
#include <string>
#include <iostream>
#include <vector>
using std::string;
using std::cin;
using std::endl;
using std::cout;
using std::vector;
enum ServerState {
	Leader, Candidate, Follower
};