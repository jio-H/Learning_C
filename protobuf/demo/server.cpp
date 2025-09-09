#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include "../message.pb.h"

using namespace std;

void read_cb(struct bufferevent *bev, void *ctx) {
    evbuffer *input = bufferevent_get_input(bev);
    size_t len = evbuffer_get_length(input);
    vector<char> buffer(len);
    evbuffer_copyout(input, buffer.data(), len);

    Message message;
    //使用Protocol Buffers解析数据
    if (message.ParseFromArray(buffer.data(), len)) {
        cout << "Received: " << message.content() << endl;
    }

    bufferevent_free(bev);//释放缓冲区数据
}

void accept_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *ctx) {
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, read_cb, NULL, NULL, NULL);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}

int main() {
    struct event_base *base;
    struct evconnlistener *listener;
    struct sockaddr_in sin;

    base = event_base_new();
    if (!base) {
        cerr << "Could not initialize libevent!" << endl;
        return 1;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(8080);

    listener = evconnlistener_new_bind(base, accept_cb, NULL,
                                       LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1,
                                       (struct sockaddr *)&sin, sizeof(sin));
    if (!listener) {
        cerr << "Could not create a listener!" << endl;
        return 1;
    }

    event_base_dispatch(base);
    evconnlistener_free(listener);
    event_base_free(base);

    return 0;
}