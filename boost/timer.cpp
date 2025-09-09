#include<boost/heap/priority_queue.hpp>
#include<iostream>
#include<chrono>
#include<thread>
#include<functional>

struct TimeEvent {
    int time;
    std::function<void()> callback;

    TimeEvent(int &time_, std::function<void()> cb) 
    : time(time_), callback(std::move(cb)) {}

    bool operator
};

int main() {

}
