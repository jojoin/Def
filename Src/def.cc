/*
	Minimal main program -- everything is loaded from the library
*/



#include <iostream>
#include <cstdlib>
#include <string>

#include "./vm/exec.h"
#include "./object/object.h"

using namespace std;
using namespace def::vm;
using namespace def::object;


int main(int argc, char *argv[])
{
    // cout << "argc= " << argc << endl;

    if(argc==1){
        // 显示简介
        cout<<"Welcome to use Def !"<<endl;
        cout<<"-c   "<<endl;
        
    }else if(argc>1){

        string cmd(argv[1]);

        // 动态交互环境
        if(cmd=="-c"){
            cout<<"Input your code, enter to run (quit to end):"<<endl;
            // 动态执行环境
            Exec exec = Exec(); // 初始化
            string input;
            while(1){
                cout<<">>>";
                getline(cin, input);
                if(input=="quit"){
                    break;
                }
                // cin >> input;
                // cout<<input<<endl;;
                DefObject* res = exec.Eval(input); // 执行
                if(res){
                    DefObject::Print(res); //打印
                    cout<<endl;
                }
            }

        // 解析执行文件
        }else{
            // cout << "code file is " << argv[1] << endl;
            Exec exec = Exec(); // 初始化
            return exec.Main(argv[1]); // 入口文件执行

        }


    }


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


