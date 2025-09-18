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

struct buffer{
    int x;
    explicit buffer(int x_) : x(x_) {}
};
std::optional<buffer> get_(buffer buf) {
    if(buf.x == 0) return {};
    else return buf;
}
int main() {
    
    buffer x(10);
    get_(x);
    
    std::vector<int> fds;
    
    std::optional<int> opt; // 默认构造，不包含值

    if (!opt) {
        std::cout << "opt is empty\n";
    }

    opt = 5; // 赋值

    if (opt) {
        std::cout << "opt contains: " << *opt << "\n"; // 使用*操作符访问值
    }

    opt.reset(); // 清空 optional

    if (!opt) {
        std::cout << "opt is now empty again\n";
    }
    
    for(int i=0; i<10; ++i) {
        fds.push_back(socket(AF_INET, SOCK_STREAM, 0));
    }
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 任意地址
    address.sin_port = htons(8000);
    for(int i=0; i<1; i++) {
        if(connect(fds[i], (struct sockaddr*)&address, sizeof(address)) == -1) {
            std::cout<<"connect 出错"<<std::endl;
            close(fds[i]);
        }
        else cout << fds[i] << endl;
        while(1) {
            // char s[20];
            // scanf("%s", s);
            string ss;
            cin >> ss;
            send(fds[i], &ss, sizeof(ss), 0);
            read(fds[i], &ss, sizeof(ss));
            cout << ss << endl;
        }
    }
    return 0;
}