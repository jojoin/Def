/*
	Minimal main program -- everything is loaded from the library
*/


#include <iostream>
#include <string>

//#include "Def.h" //wchar_t 

using namespace std;

int main()
{
    int i, j;
    double d;
    string s;  // C++中新增 string 类型

    i = 10;
    d = 123.45;
    s = "http://see.xidian.edu.cn/cpp/biancheng/cpp/rumen/";

    cout << "请输入一个整数：";
    cin >> j;
    cout << "i=" << i << "\n";
    cout << "j=";
    cout << j;
    cout << endl;
    cout << "d=" << d << endl;
    cout << s << endl;

    return 0;
}


