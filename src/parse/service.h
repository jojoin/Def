#pragma once


#include <set>
#include <list>
#include <vector>
#include <string>

#include "../core/ast.h"
#include "../core/type.h"
#include "./element.h"
#include "./stack.h"
#include "./tokenizer.h"


namespace def {
namespace parse {
    
using namespace std;
using namespace def::core;


#define Namespace vector<string>


/**
 * 语法分析服务类
 */
class Service
{
    

public:

    Service(Tokenizer * t);
    
    Tokenizer * tkz; // 词法分析器
    
    Stack * stack; // 当前分析栈
    map<TypeStruct*, Stack*> type_member_stack; // 类成员栈
    map<TypeStruct*, ASTTypeDefine*> type_define;  // 类成员栈

    // 获取一个单词
    Tokenizer::Word getWord();
    void prepareWord(const Tokenizer::Word &); // 预备
    void prepareWord(list<Tokenizer::Word>); // 拷贝预备


public: // 功能型函数
    
    static Type* getType(AST*); // 类型获取
    static bool checkType(Type*, AST*); // 类型判断

public:

    string fixNamespace(const string &); // 追加名字空间
    // 检查并设置 include 文件的绝对路径
    bool checkSetInclude(const string&);

    // 比较函数返回值类型
    void verifyFunctionReturnType(Type*); 
    
public:
    
    // 是否为构造函数状态
    bool status_construct = false;
    
public:
    // 缓存/预备的单词
    list<Tokenizer::Word> prepare_words; 

    set<string> includes; // 已经 include 包含的文件

    string defspace;       // 当前定义的名字空间
    Namespace usespaces;   // 正在使用的名字空间
    
};



}
}

