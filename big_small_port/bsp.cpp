#include<iostream>

using namespace std;

union {
    unsigned char a[4];
    unsigned int b;  
} test_end;

int main() {
    test_end.b = 0x12345678;
    if(test_end.a[0] == 0x12) {
        cout << "大端序\n";
    }
    else if(test_end.a[0] == 0x78) {
        cout << "小端序\n";
    } 
    else {
        cout << "无法判断\n";
    }
    return 0;
}