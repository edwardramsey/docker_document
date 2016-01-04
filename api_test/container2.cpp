#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string>
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

const string GetContainers()
{
	string strCmd = "curl localhost:4243/containers/json?all=0";
	return ExecShell(strCmd);
}

const string GetTargetContainer()
{
	string strCmd = "curl localhost:4243/containers/json?all=1|grep container1";
	return ExecShell(strCmd);
}

const string GetVersion()
{
	string strCmd = "curl localhost:4243/version";
	return ExecShell(strCmd);
}

const string InspectApp(const string& id)
{
	string strCmd = ("curl localhost:4243/containers/"+id) + "/json";
	return ExecShell(strCmd);
}

int ModifyApp(bool isNeedStart, const string& id)
{
	string strCmd = ("curl -v -X POST localhost:4243/containers/"+id) + (isNeedStart?"/start":"/stop");
	ExecShell(strCmd);
	return 0;
}

const string CreateApp()
{
	string strCmd = "curl -X POST -H \"Content-Type: application/json\" -d "
		"'{\"Image\":\"ubuntu\", \"Cmd\":[\"date\"], \"OpenStdin\":true, \"Tty\":true, \"StdinOnce\": true, \"Name\":\"/containerTest\"}'"
		" localhost:4243/containers/create?name=container_test";
	return ExecShell(strCmd);
}

int main()
{
	string target = "localhost:4243";
	string result;
	string strCmd = "command -v curl";
	result = ExecShell(strCmd);
	if(!result.empty())
	{
		cout << "docker version info: " << GetVersion()    << endl;
		cout << "======================================="  << endl;
		cout << "docker containers: " 	<< GetContainers() << endl;
		// 木有用json库，直接强转 TODO
		// Example: 
		// {"Command":"/bin/container1","Created":1451547821,
		// "Id":"1ad9731c6104ad852472261abf84b0ea168dd7e7dd5605031bfe1747ec551275",
		// "Image":"container1:v1","Labels":{},"Names":["/container1"],"Ports":[],"Status":"Up Less than a second"}
	
		string containerinfo = GetTargetContainer();
		string con1Id = containerinfo.substr(containerinfo.find("Id\":\"")+5, 12); 
		cout << "docker container1: " << InspectApp(con1Id) << endl;

		sleep(1);
		cout << "stop a container" << endl;
		ModifyApp(false, con1Id);
		sleep(2);
		cout << "docker containers: " 	<< GetContainers() << endl;
		sleep(2);
		cout << "======================================="  << endl;

		ModifyApp(true, con1Id);
		cout << "docker containers: " 	<< GetContainers() << endl;

		cout << "docker create container:" << CreateApp() << endl;
	}
	else
	{
		cout << "no cmd curl" << endl;
	}
}
