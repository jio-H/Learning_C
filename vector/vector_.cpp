#include<iostream>
#include <new>

template<class T>
class vector_{
private:
    T* data = nullptr;
    size_t size = 0;
    size_t capacity = 0;
public:
    vector_()=default;
    
    ~vector_(){
        clear();
        dellocate();
    }
    
    void clear() {
        for(size_t i=0; i<size; ++i) {
            data[i].~T();
        }
        size = 0;
    }
    void push_back(const T& obj) {
        emplace_back(obj);
    }
    void push_back(T&& obj) {
        emplace_back(std::move(obj));
    }
    template<typename... Args>
    void emplace_back(Args&&... args){
        if(size == capacity) {
            reallocate(capacity == 0 ? 1 : capacity * 2);
        }
        new (data + size) T(std::forward<Args>(args) ... );
        return data[size++];
    }strcap
    void pop_back() {
        if(size > 0) data[--size].~T();
    }
private:   
    void* allocate(size_t capacity) {
        return static_cast<T*>(::operator new(capacity * sizeof(T)));
    } 
    void dellocate() {
        if(data) {
            ::operator delete(data);
            data = nullptr;
            capacity = 0;
        }
    }
    void reallocate(size_t new_cap) {
        T* new_data = allocate(new_cap);
        
        size_t new_size =  0;
        try{
            for(; new_size<size; ++new_size) {
                new(new_data + new_size) T(std::move_if_noexcept(data[new_size]));
            }
        }
        catch(...){
            for(size_t i = 0; i < new_size; ++i) {
                new_data[i].~T[i];
            }
            ::operator delete(new_data);
            throw;
        }
        clear();
        dellocate();
        
        data = new_data;
        size = new_size;
        capacity = new_cap;
    }
};

int main() {
    
    return 0;
}