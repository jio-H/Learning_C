#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <vector>
#include <optional>

using namespace std;

int main() {
    
    std::vector<int> fds;
    
    for(int i=0; i<10; ++i) {
        fds.push_back(socket(AF_INET, SOCK_STREAM, 0));
    }
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 任意地址
    address.sin_port = htons(8080);
    for(int i=0; i<10; i++) {
        if(connect(fds[i], (struct sockaddr*)&address, sizeof(address)) == -1) {
            std::cout<<"connect 出错"<<std::endl;
            close(fds[i]);
        }
        else cout << fds[i] << endl;
    }
    
    return 0;
}