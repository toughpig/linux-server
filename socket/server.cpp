#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unordered_map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <mutex>
#include <pthread.h>

using namespace std;

#define PORT 8888
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

unordered_map<int, int> clientFd;
int epoll_fd;
int server_fd;
mutex mtx_clientFd;
mutex mtx_epollFd;

void* handle_client_msg(void *_)
{
    struct epoll_event events[MAX_CLIENTS];
    char buffer[BUFFER_SIZE] = {0};

    // 等待客户端连接
    while(1) {
        // 等待事件
        // mtx_epollFd.lock();
        int n_events = epoll_wait(epoll_fd, events, MAX_CLIENTS, 1000);
        // mtx_epollFd.unlock();

        for (int i = 0; i < n_events; i++) {
            mtx_clientFd.lock();
            if ((events[i].events & EPOLLIN)) {
                int _fd = events[i].data.fd;
                if (clientFd.find(_fd) != clientFd.end())
                {
                    // 接收数据
                    read(_fd, buffer, BUFFER_SIZE);
                    printf("%s\n", buffer);

                    if (strcmp(buffer, "quit") == 0)
                    {
                        strcpy(buffer, "close socket from server");
                        send(_fd, buffer, sizeof(buffer), 0);
                        close(_fd);
                        clientFd.erase(_fd);
                    }

                    strncat(buffer, " from server", 12);
                    // 回复消息
                    send(_fd, buffer, strlen(buffer), 0);

                    memset(buffer, 0, sizeof(buffer));
                }
            }
            mtx_clientFd.unlock();
        }
        mtx_clientFd.lock();
        if(clientFd.empty())
        {
            mtx_clientFd.unlock();
            break;
        }
        mtx_clientFd.unlock();
    }

    return nullptr;
}

void* handle_client_connect(void* _)
{
    struct sockaddr_in client_address;
    socklen_t client_addrlen = sizeof(client_address);

    int client_fd;
    bool need_msg_thread = true;
    pthread_t msg_thread;

    while (true)
    {
        if ((client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            mtx_clientFd.lock();
            if (clientFd.find(client_fd) != clientFd.end())
            {
                printf("already have client_fd %d\n", client_fd);
            }
            else
            {
                clientFd[client_fd] = 1;
                printf("New connection from client, fd %d\n", client_fd);

                // 注册事件
                struct epoll_event event = { .events = EPOLLIN, .data = { .fd = client_fd } };
                mtx_epollFd.lock();
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                    perror("epoll_ctl failed");
                    exit(EXIT_FAILURE);
                }
                void *ptstatus = nullptr;
                // pthread_tryjoin_np(msg_thread, &ptstatus);
                if (need_msg_thread || 0 == pthread_tryjoin_np(msg_thread, &ptstatus))
                {
                    need_msg_thread = false;
                    int rc = pthread_create(&msg_thread, NULL, handle_client_msg, NULL);
                    if (rc != 0) {
                        printf("Error creating thread: %d\n", rc);
                        return nullptr;
                    }
                    else
                    {
                        printf("create msg thread %d\n", msg_thread);
                    }
                }
                mtx_epollFd.unlock();
            }
            mtx_clientFd.unlock();
        }
    }

    pthread_join(msg_thread, NULL);

    return nullptr;
}

int main() {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char* message = "Hello from server";

    // 创建socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 绑定socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 监听socket
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    epoll_fd = epoll_create1(0);

    // 连接处理线程
    pthread_t pt;
    int rc = pthread_create(&pt, NULL, handle_client_connect, NULL);
    if (rc != 0) {
        printf("Error creating thread: %d\n", rc);
        return -1;
    }

    pthread_join(pt, NULL);

    return 0;
}
