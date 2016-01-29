#include <iostream>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <sys/time.h>
#include <vector>

using namespace std;

const string ExecShell(const string& cmd)
{
        string result = string("");
        FILE * fp;
        char buffer[2048];
        fp = popen(cmd.c_str(), "r");
        while(fgets(buffer, sizeof(buffer), fp))
        {
                result += string(buffer);
        }
        pclose(fp);
        return result;
}

void* request(void* arg)
{ struct  timeval  start;
	struct  timeval  end;
	gettimeofday(&start, NULL);

	string strCmd = "curl -s --max-time 1 localhost:4243/containers/json?all=0";
	// string strCmd = "curl localhost:4243/containers1/json?all=0";

	gettimeofday(&end, NULL);
	unsigned long timer = 1000000 * (end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);

	printf(">>>>>> thread info %u, time: [%ld], result: [%s] >>>>>>\n", 
			pthread_self(), timer, (ExecShell(strCmd)).c_str());

	if(timer > 100000)
	{
		printf(">>>>>> thread info %u, time: [%ld]  >>>>>>\n", 
	 		pthread_self(), timer);
	}
}

#define NUM 320

int main()
{
	printf("Begin the test program\n");	

	vector<pthread_t> vecThread;
	for(int i = 0; i < NUM; ++i)
	{
		pthread_t fd;
		int ret = pthread_create(&fd, NULL, request, NULL);
		if(ret != 0)
		{
			printf("create thread failed \n");
			return -1;
		}

		vecThread.push_back(fd);
	}
	
	void * retval;
	for(int i = 0; i < vecThread.size(); ++i)
	{
		pthread_join(vecThread[i], &retval);
	}

	return 0;
}
