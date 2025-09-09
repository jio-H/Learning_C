#include<iostream>
#include<functional>

using namespace std;

void func(int a, int b){
    cout << a << ' ' << b << endl;
}  

class Cla {
public:
    void mem_fun(int a, int b) {cout << "Mem: " << a << ' ' << b << endl;}
};

void call_fun(void (*f)(int, int), int x, int y) {
    f(x, y);
}

void call_Cla_fun(void (Cla::*fun)(int, int), int x, int y, Cla &obj) {
    (obj.*fun)(x, y);
}

void free_fun(int x){
    cout << "free_fun\n";
}

struct Func{
    void operator() (int a) {
        cout << "Fun\n";
    }
};

int main() {    
    // void (*fun)(int, int);
    // fun = &func;

    // fun(1, 2);

    // call_fun(fun, 44, 55);

    // void(Cla::*fun1)(int, int);
    // fun1 = &Cla::mem_fun;
    // Cla obj;
    // (obj.*fun1)(2, 3);

    // call_Cla_fun(fun1, 55, 66, obj);

    vector<std::function<void(int)>>  funs;
    funs.push_back(
        [](int x){
            cout << "lambda\n";
        }
    );
    Func x;
    funs.push_back(x);

    funs.push_back(free_fun);

    for(auto it : funs) {
        it(45);
    }

    std::function<void(int, int)> tmp = std::bind(func, 10, std::placeholders::_1);
    auto u = std::bind(func, 10, std::placeholders::_1);
    tmp(20, 10);
    u(20, 10);
    return 0;
}