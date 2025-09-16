#include<iostream>


class lr {
public:
    int x;
    lr(int& y) : x(y){
        std::cout << "l\n";
    }
    lr(int&& y) : x(y){
        std::cout << "r\n";
    }
};

template<class T>
void f(T&& a) {
    lr(std::forward<T>(a));
}


int main() {
    f(2);
    int a = 2;
    f(a);
    f(std::move(a));
    f(std::move(2));
    return 0;
}