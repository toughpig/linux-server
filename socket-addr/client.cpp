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

static char IP[]{"127.0.0.1"};
static int PORT{8889};
static int BACKLOG{5};

void connect_and_send()
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

    if(connect(sock, (struct sockaddr*)&addr,sizeof(addr))<0){
        printf("connection failed\n");
    }
    
    char msg[255];
    while(1){
        scanf("%s",msg);
        send(sock,msg,strlen(msg),0);
    }
    
    close(sock);
	
}


int main(int argc, char* argv[])
{
	if(argc>1)
	{
		if(argc!=3) printf("Usage: ./client ip port\n");
		strncpy(IP,argv[1],strlen(argv[1]));
		PORT = atoi(argv[2]);
	}

	connect_and_send();

	printf("sock closed\n");

	return 0;
}


