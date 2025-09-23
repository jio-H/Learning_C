#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main() {
    cout << "Parent process (PID: " << getpid() << ") is starting..." << endl;

    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed." << endl;
        return 1;
    } else if (pid == 0) {
        // Child Process
        cout << "--> Child (PID: " << getpid() << ") is about to run 'ls -l /'" << endl;
        
        // 第一个参数是要执行的程序名
        // 后续参数是程序的命令行参数，最后一个必须是 nullptr
        // exec 系列函数 - 执行新程序
        // fork() 创建的子进程执行的是与父进程相同的代码。如果我们希望子进程执行一个全新的程序，就需要使用 exec 系列函数。
        
        // exec 系列函数会用一个全新的程序替换当前进程的内存空间（包括代码、数据、堆栈），进程ID保持不变。一旦调用成功，原程序中 exec 调用之后的代码将永远不会被执行。
        
        // 函数族
        // exec 不是一个函数，而是一族函数，它们的命名规则反映了其参数传递方式：
        
        // l (list): 参数以可变参数列表的形式给出，以NULL结尾。
        // v (vector): 参数以一个字符串数组（char*[]）的形式给出。
        // p (path): 会在系统的PATH环境变量中搜索要执行的程序。
        // e (environment): 允许额外传递一个环境变量数组。
        // 常用组合：
        
        // execl(const char *path, const char *arg, ...)
        // execlp(const char *file, const char *arg, ...)
        // execv(const char *path, char *const argv[])
        // execvp(const char *file, char *const argv[])
        // 如果 exec 调用成功，它将不会返回。如果调用失败（例如程序不存在、没有权限），它会返回-1，并设置 errno。
    
        execlp("ls", "ls", "-l", "/", nullptr);
        
        // 如果 execlp 成功，下面的代码将不会被执行
        // 如果执行到这里，说明 execlp 失败了
        cerr << "--> execlp failed!" << endl;
        exit(1); // 必须退出，否则子进程会继续执行父进程的代码
    } else {
        // Parent Process
        cout << "Parent is waiting for the command to complete..." << endl;
        
        wait(nullptr); // 等待子进程结束，这里不关心退出状态
        
        cout << "Child has finished. Parent is exiting." << endl;
    }

    return 0;
}
