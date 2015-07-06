/*
	Minimal main program -- everything is loaded from the library
*/



#include <iostream>
#include <cstdlib>
#include <string>

#include "./global.h"

#include "./vm/exec.h"
#include "./object/object.h"

using namespace std;
using namespace def::vm;
using namespace def::object;


int main_cmd(const string &cmd);

int main(int argc, char *argv[])
{
    // cout << "argc= " << argc << endl;

    if(argc==1){
        // 显示简介
        cout<<"Welcome to use Def ( version 0.1.0 ), available parameters:"<<endl;
        cout<<"  -c      Open REPL(Read-Eval-Print Loop)"<<endl;
        cout<<"  <file>  Code file to run like 'hello.d'"<<endl;
        cout<<"New can input your order like above: "<<endl;
        string input;
        getline(cin, input);
        return main_cmd(input);
        
    }else if(argc==2){

        string cmd(argv[1]);
        return main_cmd(cmd);

    }


    return 0;
}





/**
 * 通过命令选择行为
 */
int main_cmd(const string &cmd)
{

    // 交互式解释器  REPL(Read-Eval-Print Loop)
    if(cmd=="-c"){
        cout<<"Input your code, enter to run (quit to end):"<<endl;
        // 动态执行环境
        Exec exec = Exec(); // 初始化
        string str;
        bool mul=false;   //多行模式
        while(1){
            cout<<">>>";
        	string input;
            getline(cin, input);
            str = input;
            if(input=="quit"){ // 退出
                break; 
            }else if(input==""){ // 进入多行模式
        		while(1){
	            	cout<<"   ";
		            getline(cin, input);
		            if(input==""){ // 退出多行
		            	break;
		            }
        			str += " "+input;
		        }
            }
            // cin >> input;
            // cout<<input<<endl;;
            DefObject* res = exec.Eval(str); // 执行
            if(res){
                DefObject::Print(res, true); // 递归打印
                cout<<endl;
            }
            // 缓存复位
            str = "";
        }

    // 解析执行文件
    }else{
        // cout << "code file is " << argv[1] << endl;
        Exec exec = Exec(); // 初始化
        return exec.Main( cmd ); // 入口文件执行
    }


}




