#include<iostream>
#include<functional>
#include<vector>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<queue>
#include<future>

using namespace std;
class ThreadPool{
using TaskType = std::function<void()>;
private:
    vector<thread> threads;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<TaskType> taskQueue;
    int threadNums;
    bool stopFlag = false;
public:
    ThreadPool(int threadnums) : threadNums(threadnums) {
        for(int i=0; i<threadnums; ++i) {
            threads.push_back(std::thread([this](){
                while(true) {
                    TaskType task;
                    {
                        std::unique_lock<mutex> lock(mtx);
                        cv.wait(lock, [this](){
                            return !taskQueue.empty() || stopFlag;
                        });
                        if(taskQueue.empty() && stopFlag) return ;
                        task = std::move(taskQueue.front());
                        taskQueue.pop();
                    }
                    task();
                }
            }));
        }
    }
    ~ThreadPool() {
        {
            std::unique_lock<mutex> lock(mtx);
            stopFlag = true;
        }
        cv.notify_all();
        for(int i=0; i<threadNums; ++i) {
            threads[i].join();
        }
    }
    void setStop(bool flag = true) {
        {
            std::unique_lock<mutex> lock(mtx);
            stopFlag = flag;
        }
        cv.notify_all();
    }
    template<typename T, typename... Arg>
    auto addTask(T &&func, Arg&& ... args) 
    -> std::future<decltype(func(args...))>{
        using Returntype = decltype(func(args...));

        auto task = std::make_shared<std::packaged_task<Returntype()>>(
            std::bind(std::forward<T>(func), std::forward<Arg>(args)...)
        );
        std::future<Returntype> result = task->get_future();
        {
            std::unique_lock<mutex> lock(mtx);
            taskQueue.push([task](){
                (*task)();
            });
        }
        cv.notify_one();
        return result;
    }
};

void exampleFunc(int n, int m) {
    cout << "thread = " << std::this_thread::get_id() << " n = " << n << " m = " << m << endl;
}

int main() {
    ThreadPool threadpool(3);
    std::vector<std::future<void>> results;

    for(int i=0; i<10; ++i) {
        results.emplace_back(threadpool.addTask(exampleFunc, i, i*100));
    }

    for(auto & result : results) {
        result.get();
    }
    return 0;
}
