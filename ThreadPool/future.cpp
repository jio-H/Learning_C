#include<future>
#include<thread> 
#include<chrono>
#include<iostream>


class result {
public:
    int a, b;
    result(int a_, int b_) : a(a_), b(b_) {}
};

auto to_time_t(const std::chrono::system_clock::time_point& tp) {
    return std::chrono::system_clock::to_time_t(tp);
}

result ItoI(result x) {
    auto t0 = std::chrono::system_clock::now();    
    std::cout << "ItoI start " << to_time_t(t0) << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // 再获取时间点
    auto t1 = std::chrono::system_clock::now();
    std::cout << "ItoI end " << to_time_t(t1) << std::endl;

    // 计算经过的时长
    // auto duration = t1 - t0; // type: std::chrono::duration
    // auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    // std::cout << "Elapsed: " << ms << " ms\n";
    
    return x;
}


void print() {
    auto t0 = std::chrono::system_clock::now();    
    std::cout << "print start " << to_time_t(t0) << std::endl;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    result obj(1, 2);
    std::packaged_task<result(result)> task(ItoI);
    std::future<result> fu= task.get_future();
    
    std::thread tmp = std::thread(std::move(task), obj);
    tmp.join();
    result ans = fu.get();
    
    std::cout << ans.a << ' ' << ans.b << std::endl;
    
    auto t1 = std::chrono::system_clock::now();
    std::cout << "print end " << to_time_t(t1) << std::endl;

}



int main() {
    
    std::thread tmp(print);
    tmp.join();
    
}