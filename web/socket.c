// #include<sys/ioctl.h>
#include<netinet/in.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<unistd.h>

#include<linux/nvme_ioctl.h>

#include<pthread.h>

void* clinet_thread(void *arg) {
    int clientfd = *(int *)arg;
    while(1) {
            char buffer[128] = {0};
            // 这种只能负责一个客户端的服务
            int count = recv(clientfd, buffer, 120, 0);
            // 当recv返回了0，表示对方断开连接
            if(count == 0) {
                close(clientfd);
                return NULL;
            }
            send(clientfd, buffer, count /*128*/, 0);
            printf("clientfd: %d, count: %d, buffer: %s\n", clientfd, count, buffer);
        }
    close(clientfd);
}


int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(struct sockaddr_in));

    serveraddr.sin_family = AF_INET;

    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(2048);

    if(-1 == bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr))){ //成功返回0，失败返回-1
        perror("bind");
        return -1;
    }

    if (listen(sockfd, 10) == -1) {
        perror("listen");
        
        return -1;
    }

#if 0

    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    int clientfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len);
    printf("accept\n");
#if 0
     while(1) {
        char buffer[128] = {0};
        int count = recv(clientfd, buffer, 120, 0);
        send(clientfd, buffer, count /*128*/, 0);
        printf("socket: %d, clientfd: %d, count: %d, buffer: %s\n", sockfd, clientfd, count, buffer);
    }
#else
    while(1) {
        char buffer[128] = {0};
        // 这种只能负责一个客户端的服务
        int count = recv(clientfd, buffer, 120, 0);
        // 当recv返回了0，表示对方断开连接
        if(count == 0) {
            break;
        }
        send(clientfd, buffer, count /*128*/, 0);
        printf("socket: %d, clientfd: %d, count: %d, buffer: %s\n", sockfd, clientfd, count, buffer);
    }
#endif
#else 
    while(1) {
        struct sockaddr_in clientaddr;
        socklen_t len = sizeof(clientaddr);
        int clientfd = accept(sockfd, (struct sockaddr*)&clientaddr, &len);
        
        pthread_t thid;
        pthread_create(&thid, NULL, clinet_thread, &clientfd);
    }
#endif

    getchar();
}