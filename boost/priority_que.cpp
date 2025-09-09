#include <boost/heap/priority_queue.hpp>
#include <iostream>

struct TimerEvent {
    int id;
    int time;

    TimerEvent(int id, int time) : id(id), time(time) {}

    // 注意这里使用成员函数方式重载比较操作符
    bool operator<(const TimerEvent& other) const {
        return time < other.time; // 更早的时间具有更高的优先级
    }
};

int main() {
    // 使用成员函数比较来创建优先队列
    boost::heap::priority_queue<TimerEvent> timerQueue;

    // 插入一些事件
    timerQueue.push(TimerEvent(1, 5));
    timerQueue.push(TimerEvent(2, 3));
    timerQueue.push(TimerEvent(3, 10));

    while (!timerQueue.empty()) {
        TimerEvent event = timerQueue.top();
        std::cout << "Processing timer event ID: "
                  << event.id << ", time: " << event.time << std::endl;
        timerQueue.pop();
    }

    return 0;
}