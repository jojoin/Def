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
    
    static bool checkType(Type*, AST*); // 类型判断

public:

    // string fixNamespace(const string &); // 追加名字空间
    // 检查并设置 include 文件的绝对路径
    bool checkSetInclude(const string&);

    // 比较函数返回值类型
    void verifyFunctionReturnType(Type*); 
    
    // 合法的 struct array 对象
    static TypeStruct* validTypeStruct(AST*);
    static TypeArray* validTypeArray(AST*);
    static Type* validType(AST*); // 合法的对象类型
    static Type* validType(Type*); // 合法的对象类型


public:
    
    // 是否为构造函数状态
    bool status_construct = false;
    // 模板解析状态
    string tpl_fun_name = ""; // 模板函数解析名称
    string tpl_ty_name = ""; // 模板类解析名称
    
    // 上下文状态
    bool is_mod_adt = false;  // 是否为适配器模式

public:
    
    void setModADT(bool); // 设置 adt 状态
    bool checkModADT(); // 检查并重设 adt 状态
    


    
public:
    // 缓存/预备的单词
    list<Tokenizer::Word> prepare_words; 

    set<string> includes; // 已经 include 包含的文件

    // string defspace;       // 当前定义的名字空间
    // vector<string> usespaces;   // 正在使用的名字空间
    
};



}
}

