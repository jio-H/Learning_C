#include <iostream>
using namespace std;

class Base {
public:
    virtual void foo() { cout << "Base::foo\n"; }
};

class Derived : public Base {
public:
    void foo() override { cout << "Derived::foo\n"; }
};

class Parent {
public:
    ~Parent() { cout << "/* 父类资源释放 */\n"; } // 非虚析构函数
};

class Child : public Parent {
private:
    int data;
public:
    Child() { data = 1; }
    ~Child() { cout << ">>\n";} // 子类析构函数（释放动态内存）
};

int getStatic() {
    return static int xxx = 10;
}

int main() {
    Derived d;
    
    Base b = d; // 对象切片
    b.foo();    // 输出 Base::foo，而不是 Derived::foo

    Base* pb = &d; // 指针多态
    pb->foo();     // 输出 Derived::foo

    Base& rb = d;  // 引用多态
    rb.foo();      // 输出 Derived::foo

    // int *arr = (int*)malloc(10 * sizeof(int));
    // arr[1] = 1;
    // for(int i=0; i<10; ++i) {
    //     cout << arr[i] << endl;
    // }
    // // int* arr = new int(10);
    // delete arr;
    // for(int i=0; i<10; ++i) {
    //     cout << arr[i] << endl;
    // }


    // 内存泄漏问题
    Parent* ptr = new Child(); // 父类指针指向子类对象
    delete ptr; // 仅调用 Parent::~Parent()，Child 的 data 内存泄漏

    int x = getStatic();
    cout << x << endl;
    return 0;
}
