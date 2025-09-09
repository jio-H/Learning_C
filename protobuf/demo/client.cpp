#include <iostream>
#include <event2/event.h> //libevent框架的---事件相关
#include <event2/bufferevent.h>//libevent框架的---缓冲相关
#include <arpa/inet.h> //用于定义网络地址转换函数。
#include "../message.pb.h"

using namespace std;

void event_cb(struct bufferevent *bev, short events, void *ctx) {
    if (events & BEV_EVENT_CONNECTED) {
        cout << "Connected to server" << endl;

        Message message;
        message.set_content("Hello, World!");

        string serialized;
        message.SerializeToString(&serialized);

        bufferevent_write(bev, serialized.c_str(), serialized.size());
    } else if (events & BEV_EVENT_ERROR) {
        cerr << "Connection error" << endl;
        bufferevent_free(bev);
    }
}

int main() {
    struct event_base *base;
    struct bufferevent *bev;
    struct sockaddr_in sin;

    base = event_base_new();
    if (!base) {
        cerr << "Could not initialize libevent!" << endl;
        return 1;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);

    bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, NULL, NULL, event_cb, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    if (bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        cerr << "Could not connect to server" << endl;
        bufferevent_free(bev);
        return 1;
    }

    event_base_dispatch(base);
    bufferevent_free(bev);
    event_base_free(base);

    return 0;
}