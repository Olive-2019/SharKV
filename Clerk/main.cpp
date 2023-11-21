#include "Clerk.h"
#include "ShardCtrler.h"
int main() {
	//Clerk clerk;
	ShardCtrler shardCtrler(NetWorkAddress("127.0.0.1", 8011), NetWorkAddress("127.0.0.1", 7001), 10);
}