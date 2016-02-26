#include "etcd_tool.h"

void compare(const string& a, const string& b)
{
	cout << (a == b ? "OK" : "Failed") << endl;
}


int main()
{
	Client c("10.1.245.157", 2379);

	// Get key and Get dir
	cout << c.Get("/compose_test/1_2_3/ipc") << endl;
	cout << c.Get("/compose_test/1_2_3/ipc123123") << endl;
    cout << c.Get("/compose_test/1_2_3") << endl;

	// Set value
	c.Set("/test_etcd/kaka", "kaka");
    cout << compare(c.Get("/test_etcd/kaka"), "kaka") << endl;
	// Set 路径错误
	c.Set("/test_etcd//kaka", "kaka");
	// 重复Set
	c.Set("/test_etcd/kaka", "kaka");
	c.Set("/test_etcd/kaka", "ppp");

	// Delete key/not exist / Delete dir/not exist
	c.Set("/test_etcd/ppp", 123);
	int ret = c.Delete("/test_etcd/ppp");
	cout << ret << endl;
	ret = c.Delete("/test_etcd/ppp");
	cout << ret << endl;
	c.Set("/test_etcd/ppp", 123);
	ret = c.Delete("/test_etcd");
	cout << ret << endl;

	cout << "====== list a directory =========" << endl;
    map<string, string> mp;
    c.ListDir("/compose_test/1_2_3", mp);
    map<string, string>::iterator it = mp.begin();
    for(; it != mp.end(); ++it)
    {
        cout << it->first << ", " << it->second << endl;
    }

	cout << "======== list key not exist ========" << endl;
	mp.clear();
	c.ListDir("/compose_test/1_2_22", mp);

	cout << "======== list single key ==========" << endl;
	mp.clear();
	c.ListDir("/compose_test/1_2_3/ipc", mp);
	it = mp.begin();
    for(; it != mp.end(); ++it)
    {
        cout << it->first << ", " << it->second << endl;
    }

	Client c1("110.1.245.157", 2379);
	c1.Get("/dasd");
}
