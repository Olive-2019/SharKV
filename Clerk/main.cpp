#include "Clerk.h"
int main() {
	Clerk clerk(NetWorkAddress("127.0.0.1", 8011), NetWorkAddress("127.0.0.1", 7001));

	/*clerk.put("Lam", "55190906");
	clerk.put("Lam", "55190906");
	clerk.put("Lam", "55190906");
	clerk.put("Lam", "55190906");
	clerk.put("somebody", "55190922");
	clerk.put("somebody", "55190922");
	clerk.put("somebody", "55190922");
	clerk.put("somebody", "55190922");
	clerk.aget("Lam");
	clerk.put("somebody", "55190922");
	clerk.append("Lam", "123");
	clerk.append("Lam", "123");
	clerk.append("Lam", "123");
	clerk.append("Lam", "123");*/
	clerk.aget("Lam");
}