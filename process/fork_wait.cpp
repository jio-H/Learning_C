#include <iostream>
#include <string>
#include <unistd.h>    // for fork, getpid, getppid
#include <sys/wait.h>  // for wait

using namespace std;

int main() {
    cout << "Main process started, PID: " << getpid() << endl;

    pid_t pid = fork();

    if (pid < 0) {
        // Error
        cerr << "Fork failed. Exiting." << endl;
        return 1;
    } else if (pid == 0) {
        // Child Process
        cout << "--> Child process started." << endl;
        cout << "--> My PID is " << getpid() << ", my parent's PID is " << getppid() << "." << endl;
        
        // 模拟子进程执行任务
        sleep(2);
        
        cout << "--> Child process finished." << endl;
        exit(0); // 子进程正常退出
    } else {
        // Parent Process
        cout << "Parent process continues." << endl;
        cout << "Created a child with PID: " << pid << endl;

        cout << "Parent is waiting for the child to finish..." << endl;
        int status;
        wait(&status); // 阻塞等待子进程结束

        if (WIFEXITED(status)) {
            cout << "Child process exited with status: " << WEXITSTATUS(status) << endl;
        } else {
            cout << "Child process terminated abnormally." << endl;
        }
        
        cout << "Parent process finished." << endl;
    }

    return 0;
}
