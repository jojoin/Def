/**
 * Def 虚拟机（解释器）
 */

#include <iostream>

#include "vm.h"

using namespace std;

using namespace def::token;
using namespace def::node;
using namespace def::vm;


/**
 * 运行 Def 语言脚本
 * @return 表示运行成功或失败
 */
bool Vm::Run(string path)
{
    // 词法分析
    vector<Word> words; // 词法分析结果
    Tokenizer T(true, path, words); // 初始化词法分析器
    T.Scan(); // 执行词法分析

    // 语法分析
    Nodezer N(words); // 初始化语法分析器
    Node *node = N.BuildAST(); // 解析得到语法树（表达式）

    // 解释执行分析树

    // TODO::

    /*
    cout <<
    node->Child(1)->Right()->Left()->GetName()
    << endl;
    */
    

    delete node; // 析构语法树
    
	return true;
}






/****** 脚本解释器测试 ******/


int main()
{
    cout << "\n";

    Vm v = Vm(); // 初始化引擎
    v.Run("./test.d");

    cout << "\n\n";
}


/****** 测试结束 ******/