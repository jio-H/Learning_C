#include<iostream>
// #include<typeid>

using namespace std;

#define OUTPUT(f)	cout << #f << "\t: " << typeid(f).name() << endl;
class BaseA {};
class DeriveA: public BaseA {};
 
class BaseB
{
	virtual void f(){} 
};
class DeriveB: public BaseB {};
 
int main()
{
	cout << "-------直接处理类名-------" <<endl;
 
	OUTPUT(BaseA);
	OUTPUT(DeriveA);
	OUTPUT(BaseB);
	OUTPUT(DeriveB);
 
	cout << endl << "-------基类不含虚函数-------" <<endl;
 
	BaseA baseA;
	DeriveA deriveA;
	OUTPUT(baseA);
	OUTPUT(deriveA);
	
	BaseA* pa;
	pa = &baseA;
	OUTPUT(*pa);
	OUTPUT(pa);
	pa = &deriveA;
	OUTPUT(*pa);
	OUTPUT(pa);
 
	cout << endl << "-------基类含有虚函数-------" <<endl;
 
	BaseB baseB;
	DeriveB deriveB;
	OUTPUT(baseB);
	OUTPUT(deriveB);
 
	BaseB* pb;
	pb = &baseB;
	OUTPUT(*pb);
	OUTPUT(pb);
	pb = &deriveB;
	OUTPUT(*pb);
	OUTPUT(pb);
}