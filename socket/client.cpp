#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/epoll.h>

#define PORT 8888

int main() {
    int client_fd;
    struct sockaddr_in server_address;

    char buffer[1024] = {0};
    char* message = "Hello from client";

    // 创建socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr); // IP地址

    // 连接服务器
    if (connect(client_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server...\n");

    while (scanf("%s", buffer))
    {
        send(client_fd, buffer, strlen(buffer), 0);
        if (strcmp(buffer, "quit") == 0)
        {
            close(client_fd);
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }

    return 0;
}
