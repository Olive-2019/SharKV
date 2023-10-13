#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <exception>
#include <map>
#include <sstream>
using std::stringstream;
using std::map;
using std::pair;
using std::to_string;
using std::exception;
using std::string;
using std::cin;
using std::endl;
using std::cout;
using std::vector;
typedef pair<string, int> NetWorkAddress;
enum ServerState {
	Leader, Candidate, Follower
};