#include "Clerk.h"
#include "ShardCtrler.h"
int main() {
	//Clerk clerk;
	vector<int> ports = { 7010, 7020, 7030, 7040, 7050 };
	ShardCtrler shardCtrler(NetWorkAddress("127.0.0.1", 8011), NetWorkAddress("127.0.0.1", 7001), 10, ports);
	//shardCtrler.join_(NetWorkAddress("127.0.0.1", 6000));
	//shardCtrler.printShardDistribution();
	//shardCtrler.join_(NetWorkAddress("127.0.0.1", 6001));
	//shardCtrler.printShardDistribution();
	NetWorkAddress address = shardCtrler.queryByKey_("mod");
	cout << "mod " << address.first << ' ' << address.second << endl;
	//cout << "" << shardCtrler.queryShardIDByGroupID_()
}