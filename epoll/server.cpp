 #include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10
#define PORT 8080

int main()
{
    int server_fd, new_socket, epoll_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024];

    // 创建服务器套接字
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        std::cerr << "sock 创建失败" << std::endl;
        return 1;
    }

    // 配置套接字地址
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 任意地址
    address.sin_port = htons(PORT);

    //--开始bind
    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0)
    {
        std::cerr << "bind 失败" << std::endl;
        return 1;
    }

    if (listen(server_fd, 10) < 0)
    {
        std::cerr << "bind 失败" << std::endl;
        close(server_fd);
        return 1;
    }

    //--进入正题： epoll
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        std::cerr << "epoll_create1 失败" << std::endl;
        close(server_fd);
        return 1;
    }

    //添加服务器套接字到 “epoll” 中
    struct epoll_event event;
    event.events=EPOLLIN;
    event.data.fd=server_fd;

    //EPOLL_CTL_ADD 代表向epoll中 增加要监听的文件描述符
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd,&event)==-1)
    {
        std::cerr << "epoll_ctl 失败" << std::endl;
        close(server_fd);
        close(epoll_fd);
        return 1;
    }

    //创建事件数组
    struct epoll_event events[MAX_EVENTS];
       std::cout << "服务器已经运行，并且等待连接中...\n";

       while (true)//死循环
       {
          int num_events=epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
          if(num_events==-1)
          {
            std::cerr << "epoll_wait 失败" << std::endl;
            break;
          }
          for(int i=0;i<num_events;++i)
          {
            if(events[i].data.fd==server_fd)
            {
                //--有新的客户端连接
                new_socket = accept(server_fd,(struct sockaddr *)&address,(socklen_t *)&addrlen);
                if(new_socket==-1)
                {
                      std::cerr << "new_socket = accept 失败" << std::endl;
                    continue;
                }

                //添加新的连接到 epoll 中
                event.events=EPOLLIN;
                event.data.fd=new_socket;
                if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,new_socket,&event)==-1)
                {
                      std::cerr << "epoll_ctl(epoll_fd,EPOLL_CTL_ADD,new_socket,&event) 失败" << std::endl;
                    close(new_socket);
                }
                else{
                    std::cout<<"[服务器端]新连接放到epoll中成功"<<std::endl;
                }
            }
            else{
                //处理客户端的数据
                int client_socket=events[i].data.fd;
                int valread =read(client_socket,buffer,1024);

                if(valread<=0)
                {
                    //如果读取的数据为0或者小于0  那就是客户端已经端口连接了。先这么处理
                    close(client_socket);
                    epoll_ctl(epoll_fd,EPOLL_CTL_DEL,client_socket,nullptr);//从epoll中去掉这个连接
                    std::cout<<"客户端关闭连接,客户端fd="<<client_socket<<std::endl;
                }
                else{
                    //把客户端的数据打印出来
                    std::cout<<"接收："<<buffer<<std::endl;
                    send(client_socket,buffer,valread,0);//再把原封不动的数据发给客户端。算是做个响应
                }
                std::memset(buffer,0,sizeof(buffer));
            }
          }
       }

       close(server_fd);
       close(epoll_fd); //这里有个好处，不管是什么资源，都是close函数，因为他们都是基于文件描述符实现的
       //所以linux 一切皆文件，好处提现在这里。
       
    return 0;
}

