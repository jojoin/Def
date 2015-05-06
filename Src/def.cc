/*
	Minimal main program -- everything is loaded from the library
*/


#include <iostream>
#include <string>

#include "./vm/vm.h"

//#include "Def.h" //wchar_t 

using namespace std;
using namespace def::vm;


int main(int argc, char *argv[])
{
    //cout << "argc= " << argc << endl;

    // 文件参数
    if(sizeof(argv)>1){
        // cout << "code file is " << argv[1] << endl;
        Vm v = Vm(); // 初始化引擎
        v.Eval(argv[1], true);
        return 0;
    }


    cout << "Welcome to use Def !" << endl;

    return 0;

    /*
    //参数个数如下，其中第一个参数为当前可执行程序
    printf("param count is %d\n", argc);
    for(int i = 0; i < argc; ++i)
    {
        //依次输出传入参数
        printf("param %d is %s\n",(i+1), argv[i]);
    }
    return  0;
    */



    //Vm v = Vm(); // 初始化引擎
    //v.Eval("test.d", true);

    //cout << "\nyangjie!!!!\n";

    /*
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

    */
}


