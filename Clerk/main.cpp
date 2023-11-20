#include "Clerk.h"

int main() {
	vector<int> crudPorts = { 7010, 7020, 7030, 7040 };
	Clerk clerk(NetWorkAddress("127.0.0.1", 8011), NetWorkAddress("127.0.0.1", 7001), crudPorts);
}