#include "Clerk.h"
int main() {
	Clerk clerk(NetWorkAddress("127.0.0.1", 8011));

	clerk.put("Lam", "55190906");
	clerk.put("Lam", "55190906");
	clerk.put("Lam", "55190906");
	clerk.put("Lam", "55190906");
	clerk.put("somebody", "55190922");
	clerk.put("somebody", "55190922");
	clerk.put("somebody", "55190922");
	clerk.put("somebody", "55190922");
	clerk.put("somebody", "55190922");
	clerk.append("Lam", "123");
	clerk.append("Lam", "123");
	clerk.append("Lam", "123");
	clerk.append("Lam", "123");
	//cout << clerk.get("Lam") << endl;
}