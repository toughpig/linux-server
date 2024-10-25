#include<cstdio>
#include<sys/types.h>
#include<sys/socket.h>
#include<bits/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

//-----------------------------ADDR--------------------------------
//socket通用接口参数类型
#include<bits/socket.h>
struct sockaddr{
	sa_family_t sa_family;    //AF_UNIX AF_INET AF_INET6
	char sa_data[14];         //端口port + 地址ip
}


//TCP/IP专用参数类型	强制类型转换成    sockaddr_in ---> sockaddr
struct sockaddr_in{
	sa_family_t sin_family;
	u_init16_t sin_port;	//ipv4端口号
	struct in_addr sin_addr;
}

typedef uint32_t in_addr_t;
struct in_addr
{
    in_addr_t s_addr;
};

int inet_aton (const char *__cp, struct in_addr *__inp) __THROW;
char* inet_ntoa (struct in_addr __in) __THROW;

//======================================server=========================================

//-----------------------------create socket--------------------------------
//ret sockfd
int socket(int domain, int type, int protocol);


//-----------------------------BIND--------------------------------
//ret 0 succeed
int bind(int sockfd, const struct sockaddr* , socklen_t );


//-----------------------------listen--------------------------------
//ret 0 succeed
int listen(int sockfd, int backlog);


//-----------------------------accept--------------------------------
//blocking
int accept(int sockfd, struct sockaddr *addr, socklen_t* addrlen);


//======================================client=========================================

//------------------------------connect---------------------------------
//ret 0 succeed
int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);

//-------------------close-----------------------
//fd-ref-count - 1
int close(int fd);

//==================================transfer data==================================
//--------tcp---------
ssize_t recv(int sockfd, void* buf, size_t len, int flags);
ssize_t send(int sockfd, const void* buf, size_t len, int flags);

//--------udp---------
ssize_t recvfrom(int sockfd, void* buf, size_t len, int flags, 
					struct sockaddr* src_addr, socklen_t* addrlen);
ssize_t sendto(int sockfd, const void* buf, size_t len, int flags,
					const struct sockaddr* dest_addr, socklen_t addrlen);

//--------general--------
ssize_t recvmsg(int sockfd, struct msghdr* msg, int flags);
ssize_t sendmsg(int sockfd, struct msghdr* msg, int flags);


