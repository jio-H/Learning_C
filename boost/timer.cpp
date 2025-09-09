#include<boost/heap/priority_queue.hpp>
#include<iostream>
#include<chrono>
#include<thread>
#include<functional>
#include<mutex>
#include<thread> 
#include<condition_variable> 
#include<vector>
#include<atomic>

#include<filesystem>
#include<fstream>

struct TimeEvent {
    int time;
    std::function<void()> callback;

    TimeEvent(int time_, std::function<void()> cb) 
    : time(time_), callback(std::move(cb)) {}

    // 默认大根堆，取反变成小根堆
    bool operator < (const TimeEvent & other) const {
        return time > other.time;
    }
};

int current_time() {
    static auto start_time = std::chrono::steady_clock::now();
    auto now_time = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now_time-start_time).count();

}

class ThreadPool{
private:
    std::mutex mtx;
    std::vector<std::thread> workers;
    std::condition_variable cv;
    boost::heap::priority_queue<TimeEvent> timeQ;
    bool stop;
    std::atomic<int> timeAdjustment{0};

public:
    ThreadPool():stop(false){}

    void start(std::size_t numThreads) {
        for(std::size_t i=0; i<numThreads; ++i) {
            workers.emplace_back([this](){
                this->workerThread();
            });
        }
    }
    // 生产者消费者模型，线程执行的函数
    void workerThread() {
        while(true) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this](){
                return !this->timeQ.empty() || this->stop;
            });
            
            if(this->stop && this->timeQ.empty()) return ;

            auto now = current_time();
            auto& nextEvent = this->timeQ.top();

            if(now > nextEvent.time + timeAdjustment) {
                std::function<void()> task(std::move(nextEvent.callback));
                this->timeQ.pop();
                lock.unlock();
                task();
                std::cout<<"workerThread 达到执行时间，取出事件并执行回调函数timeQueue.size()="<<timeQ.size()<<std::endl; 
            }
            else {
                std::cout<<"[未达到运行条件]workerThread删除任务检查now="<<now<<std::endl; 
                std::cout<<"[未达到运行条件]workerThread删除任务检查nextEvent.time="<<nextEvent.time<<std::endl; 
                std::cout<<"[未达到运行条件]workerThread删除任务检查timeAdjustment.load()="<<timeAdjustment.load()<<std::endl; 
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::seconds((nextEvent.time + timeAdjustment.load()) - now));
            }
        }
    }

    void enqueue(TimeEvent&& event) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            timeQ.push(std::move(event));
        }
        cv.notify_one();
    }
    // 使用原子操作，避免多线程修改导致数据错乱
    void adjustTiming(int adjustment) {
        timeAdjustment.store(adjustment);
    }

    ~ThreadPool() {
        stop = true;
        cv.notify_all();
        for(std::thread& worker:workers) {
            worker.join();
        }
    }
};

// 文件操作
void clear_logs(const std::string& log_directory) {
    try {
        for(const auto& entry : std::filesystem::directory_iterator(log_directory)) {
            std::filesystem::remove_all(entry.path());
            std::cout << entry.path() << "删除成功\n";
        }
    }
    catch(const std::filesystem::filesystem_error & e){
        std::cerr << "清理日志文件出现异常："<<e.what()<<std::endl;
    }

}

void setup_log_directory(const std::string& log_directory) {
    std::cout<<"---------------------------------------------------------"<<std::endl;
    std::string log_file = log_directory+"/example.log";
    std::ofstream(log_file) << "创建成功\n";

    std::cout << "打印已创建成功的目录结构：\n";
    for(const auto & entry : std::filesystem::directory_iterator(log_directory)) {
        std::cout << entry.path() << std::endl;
    }
    std::cout<<"---------------------------------------------------------"<<std::endl;
}

// #define one_thread

int main() {



#ifdef one_thread

    boost::heap::priority_queue<TimeEvent> timeQ;

    timeQ.emplace(5, [](){std::cout<<"5, run\n";});
    timeQ.emplace(2, [](){std::cout<<"2, run\n";});
    timeQ.emplace(100, [](){std::cout<<"100, run\n";});
    while(!timeQ.empty()) {
        const TimeEvent& nextEvent = timeQ.top();
        int eventTime = nextEvent.time;
        int currentTime = current_time();

        if(currentTime >= eventTime) {
            nextEvent.callback();

            timeQ.pop();
        }
        else {
            std::this_thread::sleep_for(std::chrono::seconds(eventTime-currentTime));
        }
    }
#else
    ThreadPool pool;
    pool.start(4);

    int clear_interval_seconds = 24*60*60;
    clear_interval_seconds = 10;

    std::string log_directory = "./log";
    setup_log_directory(log_directory);

    auto clear_log_task = [&log_directory] () {
        clear_logs(log_directory);
    };

    int now = current_time();

    int next_clear_time = now + clear_interval_seconds;

    pool.enqueue(TimeEvent(next_clear_time, clear_log_task));

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        int elapsed_time = current_time() - now;

        if(elapsed_time >= clear_interval_seconds) {
            setup_log_directory(log_directory);
            next_clear_time = current_time() + clear_interval_seconds;
            pool.enqueue(TimeEvent(next_clear_time, clear_log_task));
            std::cout<<"达到填充任务间隔时间，往定时任务中增加任务:next_clear_time="<<next_clear_time<<std::endl; 
            now = current_time();
        }
        else{
            std::cout<<"循环检查，未达到填充任务间隔时间elapsed_time="<<elapsed_time
            <<"clear_interval_seconds="<<clear_interval_seconds<<std::endl; 
        }
    }


#endif

    
}
