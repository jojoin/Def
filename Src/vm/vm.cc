/**
 * Def 虚拟机（解释器）
 */

#include <iostream>
#include <cstdlib>

#include "vm.h"

using namespace std;

using namespace def::parse;
using namespace def::object;

namespace def {
namespace vm {

// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);


Vm::Vm(){
    _gc = new Gc();      // 新建对象分配
    _mod = new Module(); // 模块加载缓存
}


/**
 * 运行时错误
 */
bool Vm::Error(int code){

    // string msg = filepath + " ("
    //     +Str::l2s(line) + ","
    //     +Str::l2s(word_pos)
    //     +") : " + tok;
    // //tok;
    // return Error::Throw(ErrorType::Run, code, msg);
    return true;
}



/**
 * 运行 Def 语言脚本
 * @return 表示运行成功或失败
 */
bool Vm::Eval(string txt, bool ispath=false)
{
    // 词法分析结果
    vector<Word>* words = new vector<Word>(1024);
    Tokenizer T(ispath, txt, words); // 初始化词法分析器
    T.Scan(); // 执行词法分析

    // cout << words->at(0).value << endl;
    // cout << words->at(1).value << endl;
    // cout << words->at(2).value << endl;

    // 语法分析
    Nodezer N(words, ispath ? txt : ""); // 初始化语法分析器
    Node* node = N.BuildAST(); // 解析得到语法树（表达式）

    // cout << node->Child(0)->GetName() << endl;
    // cout << node->Right()->Child(1)->Left()->GetName() << endl;

    // node->Print();
    // cout<<endl;
    // cout << "node->ChildSize() = " << node->ChildSize() << endl;
    
    // 初始化
    _exec = new Exec(node, _gc, _mod);
    
    // 解释执行
    bool done = _exec->Run();

    delete words; // 析构words数组
    delete node; // 析构语法树
    
	return true;
}









} // end namespace vm
} // end namespace def
