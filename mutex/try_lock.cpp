#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::timed_mutex mtx;

void thread_function(int id) {
    std::unique_lock<std::timed_mutex> lock(mtx, std::defer_lock);
    if (lock.try_lock_for(std::chrono::milliseconds(100))) { // 尝试锁定
        std::cout << "Thread #" << id << " has acquired the lock" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } else {
        std::cout << "Thread #" << id << " could not acquire the lock" << std::endl;
    }
}

int main() {
    std::thread threads[5];
    for (int i = 0; i < 5; ++i) {
        threads[i] = std::thread(thread_function, i + 1);
    }

    for (auto& th : threads) {
        th.join();
    }

    return 0;
}