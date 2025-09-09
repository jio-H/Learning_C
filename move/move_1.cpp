#include <iostream>
#include <vector>

using namespace std;

class MyClass
{
public:
	MyClass(const std::string& s)
		: str{ s }
	{
	};
	void print()
	{
		std::cout << str << std::endl;
	}

private:
	std::string str;
};

int main()
{
	std::vector<MyClass> myClasses;
	MyClass tmp{ "hello" };
	myClasses.push_back(tmp);
	myClasses.push_back(std::move(tmp));
	for (auto& mc : myClasses)
		mc.print();
	int a = 5; // a是个左值
	tmp.print();//输出为空
	int& ref_a_left = a; // 左值引用指向左值
	int&& ref_a_right = std::move(a); // 通过std::move将左值转化为右值，可以被右值引用指向
	cout << a;	//输出 5
}
