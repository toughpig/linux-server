#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<sys/types.h>
#include<sys/socket.h>
#include<bits/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cassert>
#include<csignal>
#include<unistd.h>
#include "coroutine.h"

static char IP[]{"127.0.0.1"};
static int PORT{8889};
static int BACKLOG{5};

int bind_and_listen()
{
	int port = PORT;
	int backlog = BACKLOG;
//ADDR
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	inet_aton(IP,&addr.sin_addr);
	addr.sin_port = htons(port);
//socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	assert(sock>=0);
//bind
	int ret = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
	assert(ret!=-1);
//listen
	ret = listen(sock, backlog);
	assert(ret!=-1);
	
	return sock;
}

static void* coro_stat = nullptr;

void enter_coro(void **c)
{
	reenter{
		printf("first enter\n");
		yield(1000);
		printf("second enter\n");
		yield(1000);
		printf("third enter\n");
	}
	end_coro;
}

void accept_and_receive(int sock)
{
	struct sockaddr_in client;
	socklen_t client_len = sizeof(client);
	int connfd = accept(sock,(struct sockaddr*)&client,&client_len);
	assert(connfd>=0);
	printf("accept\n");
	printf("client addr: %s:%d\n",inet_ntoa(client.sin_addr),client.sin_port);
	char msg[255];

	int ret;
	while( (ret = recv(connfd,msg,254,0)) > 0 ){
		printf("get %d bytes of message %s\n",ret,msg);
		if (strcmp(msg, "coro") == 0)
		{
			enter_coro(&coro_stat);
		}else if(strcmp(msg, "quit") == 0 || strcmp(msg, "exit") == 0)
		{
			break;
		}
		memset(msg,0,255);
	}
}

int main(int argc, char* argv[])
{
	if(argc>1)
	{
		if(argc!=3) printf("Usage: ./server ip port\n");
		strncpy(IP,argv[1],strlen(argv[1]));
		PORT = atoi(argv[2]);
	}

	int sock = bind_and_listen();

	accept_and_receive(sock);

	close(sock);

	printf("sock closed\n");

	return 0;
}