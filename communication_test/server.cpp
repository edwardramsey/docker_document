#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT  4455

int main()
{
	char *addr;
	int listenfd, clientfd;
	int new_fd;
	struct sockaddr_in ser_addr;


	listenfd = socket(AF_INET, SOCK_STREAM, 0);          

	if (listenfd == -1) {
		printf("get socket fd error\n");
		exit(1);
	}

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;                
	ser_addr.sin_port = htons(SERVER_PORT);           
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); 

	int res = bind(listenfd, (struct sockaddr*)&ser_addr, sizeof(struct sockaddr));
	if (res == -1) {
		printf("bind socket fd error\n");
		exit(0);
	}

	res = listen(listenfd, 10);  
	if (res == -1) {
		printf("listen port %d failed\n", SERVER_PORT);
		exit(0);
	}

	clientfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

	char* buf = new char[255];

	while(recv(clientfd, buf, 255, 0) != -1)
	{
		printf("recv data:%s\n", buf);
		if(0 == strcmp(buf, "bye"))
		{
			break;
		}
		sleep(1);
		memset(buf, 0, 255);
	}

	close(clientfd);
	close(listenfd);

	printf("recv data:%s\n", buf);

	delete []buf;
	return 0;
}
