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
}


