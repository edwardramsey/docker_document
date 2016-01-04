#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>


#define DEST_IP   		"127.0.0.1"
#define DEST_PORT 		4455

int main()
{
	struct sockaddr_in dest_addr;

	const char *msg = "test msg from client\n";

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("get socket fd error: %s\n", strerror(errno));
		exit(0);
	}

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(DEST_PORT);
	dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);

	int ret = connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in));
	if (ret < 0) {
		printf("connect to server failed\n");
		exit(0);
	}

	int len = strlen(msg);
	for(int i = 0; i < 10; ++i)
	{
		if(send(sockfd, msg, len, 0) < 0)
		{
			printf("send msg error \n");
		}

		sleep(1);
	}

	if(send(sockfd, "bye", strlen("bye"), 0) > 0)
	{
		printf("client send bye msg to server\n");
	}

	close(sockfd);

	return 0;
}

