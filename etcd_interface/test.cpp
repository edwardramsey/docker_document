#include "etcd_tool.h"

int main()
{
	Client c("10.1.245.157", 2379);

	cout << c.Get("/compose_test/1_2_3/ipc") << endl;
    cout << c.Get("/compose_test/1_2_3") << endl;

	c.Delete("/compose_test/1_2_3/614");

    c.Set("/compose_test/1_2_3/ipc", "host2");

    cout << c.Get("/compose_test/1_2_3/ipc") << endl;

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
}
