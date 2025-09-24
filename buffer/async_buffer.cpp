#include "log.h"
 
using namespace std;
 
// Log类的构造函数
Log::Log() {
    lineCount_ = 0; // 初始化日志行数为0
    isAsync_ = false; // 初始化异步写入标志为false
    writeThread_ = nullptr; // 初始化写入线程指针为nullptr
    deque_ = nullptr; // 初始化日志队列指针为nullptr
    toDay_ = 0; // 初始化今日日期标志为0
    fp_ = nullptr; // 初始化文件指针为nullptr
}
 
// Log类的析构函数
Log::~Log() {
    // 如果存在写入线程并且可以加入，则等待线程结束
    if(writeThread_ && writeThread_->joinable()) {
        // 循环直到队列清空
        while(!deque_->empty()) {
            deque_->flush();
        };
        // 关闭队列
        deque_->Close();
        // 等待写入线程结束
        writeThread_->join();
    }
    // 如果文件指针存在
    if(fp_) {
        // 锁定互斥锁
        lock_guard<mutex> locker(mtx_);
        // 刷新缓冲区并关闭文件
        flush();
        fclose(fp_);
    }
}
 
// 获取当前日志级别
int Log::GetLevel() {
    // 锁定互斥锁
    lock_guard<mutex> locker(mtx_);
    // 返回日志级别
    return level_;
}
 
// 设置日志级别
void Log::SetLevel(int level) {
    // 锁定互斥锁
    lock_guard<mutex> locker(mtx_);
    // 设置新的日志级别
    level_ = level;
}
 
// 初始化日志系统
void Log::init(int level, const char* path, const char* suffix,
    int maxQueueSize) {
    // 设置日志级别
    isOpen_ = true;
    level_ = level;
    // 如果队列大小大于0，则开启异步写入
    if(maxQueueSize > 0) {
        isAsync_ = true;
        // 如果队列不存在，则创建新队列
        if(!deque_) {
            unique_ptr<BlockDeque<string>> newDeque(new BlockDeque<string>);
            deque_ = move(newDeque);
            // 创建新的写入线程
            std::unique_ptr<std::thread> NewThread(new thread(FlushLogThread));
            writeThread_ = move(NewThread);
        }
    } else {
        // 否则关闭异步写入
        isAsync_ = false;
    }
    // 重置日志行数
    lineCount_ = 0;
 
    // 获取当前时间
    time_t timer = time(nullptr);
    struct tm *sysTime = localtime(&timer);
    struct tm t = *sysTime;
    // 设置日志文件路径和后缀
    path_ = path;
    suffix_ = suffix;
    // 构造日志文件名
    char fileName[LOG_NAME_LEN] = {0};
    snprintf(fileName, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s", 
            path_, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix_);
    // 设置今日日期
    toDay_ = t.tm_mday;
 
    // 锁定互斥锁
    lock_guard<mutex> locker(mtx_);
    // 清空缓冲区
    buff_.RetrieveAll();
    // 如果文件指针存在，则关闭旧文件
    if(fp_) { 
        flush();
        fclose(fp_); 
    }
    // 打开新文件
    fp_ = fopen(fileName, "a");
    // 如果文件打开失败，则创建目录后再次尝试
    if(fp_ == nullptr) {
        mkdir(path_, 0777);
        fp_ = fopen(fileName, "a");
    }
    // 断言文件指针不为空
    assert(fp_ != nullptr);
}
 
// 写入日志
void Log::write(int level, const char *format, ...) {
    // 获取当前时间
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t tSec = now.tv_sec;
    struct tm *sysTime = localtime(&tSec);
    struct tm t = *sysTime;
    va_list vaList;
 
    // 如果日期变化或达到最大日志行数，则创建新文件
    if (toDay_ != t.tm_mday || (lineCount_ && (lineCount_  %  MAX_LINES == 0))) {
        // 锁定互斥锁
        unique_lock<mutex> locker(mtx_);
        locker.unlock();
        
        // 构造新文件名
        char newFile[LOG_NAME_LEN];
        char tail[36] = {0};
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
 
        // 如果日期变化
        if (toDay_ != t.tm_mday) {
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path_, tail, suffix_);
            // 更新今日日期
            toDay_ = t.tm_mday;
            // 重置日志行数
            lineCount_ = 0;
        } else {
            // 否则更新文件名，包含行数信息
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", path_, tail, (lineCount_  / MAX_LINES), suffix_);
        }
        
        // 重新加锁
        locker.lock();
        // 刷新缓冲区
        flush();
        // 关闭旧文件
        fclose(fp_);
        // 打开新文件
        fp_ = fopen(newFile, "a");
        // 断言文件指针不为空
        assert(fp_ != nullptr);
    }
 
    // 锁定互斥锁
    unique_lock<mutex> locker(mtx_);
    // 增加日志行数
    lineCount_++;
    // 格式化日志时间
    int n = snprintf(buff_.BeginWrite(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
    // 写入时间到缓冲区
    buff_.HasWritten(n);
    // 添加日志级别标题
    AppendLogLevelTitle_(level);
 
    // 格式化日志内容
    va_start(vaList, format);
    int m = vsnprintf(buff_.BeginWrite(), buff_.WritableBytes(), format, vaList);
    va_end(vaList);
 
    // 写入日志内容到缓冲区
    buff_.HasWritten(m);
    // 添加换行符
    buff_.Append("\n\0", 2);
 
    // 如果开启异步写入且队列未满，则将日志内容推送到队列
    if(isAsync_ && deque_ && !deque_->full()) {
        deque_->push_back(buff_.RetrieveAllToStr());
    } else {
        // 否则直接写入文件
        fputs(buff_.Peek(), fp_);
    }
    // 清空缓冲区
    buff_.RetrieveAll();
}
 
// 添加日志级别标题
void Log::AppendLogLevelTitle_(int level) {
    // 根据日志级别添加相应的标题
    switch(level) {
    case 0:
        buff_.Append("[debug]: ", 9);
        break;
    case 1:
        buff_.Append("[info] : ", 9);
        break;
    case 2:
        buff_.Append("[warn] : ", 9);
        break;
    case 3:
        buff_.Append("[error]: ", 9);
        break;
    default:
        buff_.Append("[info] : ", 9);
        break;
    }
}
 
// 刷新缓冲区，将内容写入文件
void Log::flush() {
    // 如果开启异步写入，则调用队列的flush方法
    if(isAsync_) { 
        deque_->flush(); 
    }
    // 刷新文件指针
    fflush(fp_);
}
 
// 异步写入日志
void Log::AsyncWrite_() {
    // 从队列中取出日志内容并写入文件
    string str = "";
    while(deque_->pop(str)) {
        // 锁定互斥锁
        lock_guard<mutex> locker(mtx_);
        // 将日志内容写入文件
        fputs(str.c_str(), fp_);
    }
}
 
// 获取日志实例
Log* Log::Instance() {
    // 创建并返回Log类的单例实例
    static Log inst;
    return &inst;
}
 
// 异步日志写入线程函数
void Log::FlushLogThread() {
    // 调用实例的异步写入函数
    Log::Instance()->AsyncWrite_();
}