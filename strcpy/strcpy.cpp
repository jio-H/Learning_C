#include<bits/stdc++.h>

using namespace std;
char *strcpy(char *dest, const char* src) {
    assert((dest != nullptr) && (src != nullptr));
    char* tmp = dest;
    while((*src) != '\0') {
        *dest = *src;
        dest++; src++;
        
    }
    *dest = *src;
    return tmp;
}
int main() {
    char a[] = "adfbcd";
    char b[] = "abcdef";
    cout << &a << ' ' << &b << endl;
    strcpy(b, a);
    cout << b << ' ' << a << endl;
    return 0;
}