#include<bits/stdc++.h>

using namespace std;

class Base {
    
};
class A : public Base {
public: 
    virtual void print(){}
    int x;
};
class B: public A {
  char x;
};

int main() {
    cout << sizeof(Base) << endl;
    cout << sizeof(A) << endl;
    cout << sizeof(B) << endl;
    return 0;
}