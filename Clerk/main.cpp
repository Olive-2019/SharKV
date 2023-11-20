#include "Clerk.h"
int main() {
	Clerk clerk(NetWorkAddress("127.0.0.1", 8011), NetWorkAddress("127.0.0.1", 7001));

	clerk.put("123", "55190906");
	clerk.put("13", "55190906");
	cout << clerk.get("123") << endl;
	clerk.deleteData("123");
	cout << clerk.get("123") << endl;
	cout << "here" << endl;
}