#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
// ...

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        // fork 失败，处理错误
        cerr << "fork failed!" << endl;
        exit(1);
    } else if (pid == 0) {
        // 此代码块由子进程执行
        cout << "This is the child process, PID = " << getpid() << endl;
        // ... 执行子进程的任务 ...
        exit(0); // 子进程任务完成后必须退出
    } else {
        // 此代码块由父进程执行
        cout << "This is the parent process, PID = " << getpid() << ", child PID = " << pid << endl;
        // ... 父进程可以继续执行自己的任务，或者等待子进程结束 ...
    }
}