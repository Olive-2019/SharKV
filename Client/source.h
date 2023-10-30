#pragma once
#include <iostream>
#include <vector>
#include <exception>
#include <map>
#include <sstream>
#include <future>
#include <string>
#include <thread>

#include <include/rest_rpc/rpc_server.h>
using namespace rest_rpc::rpc_service;
using std::string;
using std::to_string;
using std::stringstream;
using std::map;
using std::pair;
using std::exception;
using std::cin;
using std::endl;
using std::cout;
using std::vector;
using std::shared_future;
using std::future;
using std::async;
using std::thread;
typedef pair<string, int> NetWorkAddress;
