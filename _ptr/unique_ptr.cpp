#include<iostream>

using namespace std;

template<typename T>
class unique_ptr_ {
private:
    T* ptr;

public:
    explicit unique_ptr_(T* p = nullptr) : ptr(p) {}
    
    unique_ptr_(const unique_ptr_ &) = delete;
    unique_ptr_& operator = (const unique_ptr_ &) = delete;
    
    unique_ptr_(unique_ptr_&& p) noexcept : ptr(p.ptr) {
        p.ptr = nullptr;
    } 
    
    unique_ptr_& operator = (unique_ptr_&& p) noexcept {
        if(this != p) {
            delete ptr;
            ptr = p.ptr;
            p.ptr = nullptr;
        }
        return *this;
    }
    
    T& operator*() {
        return *ptr;
    }
    
    T* operator->() {
        return ptr;
    }
    
    T* get() {
        return ptr;
    }
    
    T* release() {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }
    
    void reset(T* p = nullptr) {
        if(ptr != nullptr) {
            delete ptr;
        }
        ptr = p;
    }
    
    ~unique_ptr_() {
        if(ptr != nullptr) {
            delete ptr;
        }
    }
};

int main() {
    unique_ptr_<int> p1(new int(42));  // 创建一个 unique_ptr

    std::cout << *p1 << std::endl;  // 输出 42

    unique_ptr_<int> p2 = std::move(p1);  // 移动所有权
    if (p1.get() == nullptr) {
        std::cout << "p1 is now nullptr" << std::endl;
    }

    std::cout << *p2 << std::endl;  // 输出 42

    p2.reset(new int(100));  // 重置指针
    std::cout << *p2 << std::endl;  // 输出 100
    return 0;
}