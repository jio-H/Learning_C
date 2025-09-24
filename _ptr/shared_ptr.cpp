#include<iostream>
#include<memory>
#include<atomic>

using namespace std;

template<typename T>
class shared_ptr_{
private:
    T* ptr;
    // 原子操作
    std::atomic<int>* ref_count;
    // 释放当前shared对象对指向对象的占有
    void release() {
        if(ref_count != nullptr) {
            (*ref_count)--;
            if(*ref_count == 0) {
                delete ptr;
                delete ref_count;
            }
        }
    }
public:
    // 各种构造函数
    // 无参构造
    shared_ptr_() : ptr(nullptr), ref_count(nullptr) {}
    
    // 参数构造
    explicit shared_ptr_(T* p) : ptr(p), ref_count(new atomic<int>(1)) {}
    
    // 析构函数
    ~shared_ptr_() {
        release();
    }
    
    // 左右值引用的拷贝构造函数
    shared_ptr_(const shared_ptr_ & p) : ptr(p.ptr), ref_count(p.ref_count) {
        if(ref_count != nullptr) {
            (*ref_count)++;
        }
    }
    shared_ptr_(shared_ptr_&& p) : ptr(p.ptr), ref_count(p.ref_count) {
        if(ref_count != nullptr) {
            (*ref_count)++;
        }
    }
    
    // 左右值引用赋值重载
    shared_ptr_& operator=(const shared_ptr_& p) {
        if(this != &p) {
            release();
            ptr = p.ptr;
            ref_count = p.ref_count;
            if(ref_count != nullptr) {
                (*ref_count)++;
            }
        }
        return *this;
    }
    shared_ptr_& operator=(shared_ptr_&& p) {
        if(this != &p) {
            release();
            ptr = p.ptr();
            ref_count = p.ref_count;
            if(ref_count != nullptr) {
                (*ref_count)++;
            }
        }
        return *this;
    }
    
    // 运算符重载
    T* operator->() {
        return ptr;
    }
    
    T& operator*() const{
        return *ptr;
    }
    int use_count() const {
        return ref_count != nullptr ? ref_count->load() : 0;
    }
};

template<typename T, typename...Args>
shared_ptr_<T> make_shared_(Args&&...args) {
    return shared_ptr_<T>(new T(forward<Args>(args) ...));
}
    
class A {
public:
    A(int a, int b) : ma(a), mb(b) {}

    void print() {
        cout << "a = " << ma << " b = " << mb << endl;
    }
private:
    int ma, mb;
};


int main() {
    shared_ptr_<int> p1(new int(42));
    
    cout << "p1 use_count: " << p1.use_count() << std::endl;
    
    {
        shared_ptr_<int> p2 = p1;  // 拷贝构造
        std::cout << "p1 use_count: " << p1.use_count() << std::endl;  // 输出引用计数
        std::cout << "p2 use_count: " << p2.use_count() << std::endl;  // 输出引用计数
    }  // p2 离开作用域，引用计数减 1
    
    std::cout << "p1 use_count: " << p1.use_count() << std::endl;  // 输出引用计数

    shared_ptr_<int> p3 = std::move(p1);  // 移动构造
    std::cout << "p1 use_count: " << p1.use_count() << std::endl;  // 输出引用计数
    std::cout << "p3 use_count: " << p3.use_count() << std::endl;  // 输出引用计数
    
    shared_ptr_<A> pa = make_shared_<A>(1, 2);
    pa->print();

    return 0;
}