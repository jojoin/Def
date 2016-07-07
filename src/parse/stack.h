#pragma once


#include <vector>
#include <tuple>
#include <list>
#include <set>
#include <string>

#include "./element.h"


namespace def {
namespace parse {

using namespace std;


#define ElementStack map<string, Element*>


/**
 * Def block 分析块
 */
class Stack
{
public:

    enum class Mod
    {
        Namespace,   //   名字空间
        Class,       // % 类型栈
        Function,    // @ 函数栈
        Anonymous,   // # 匿名栈
    } mod = Mod::Anonymous;
    
    Stack* parent = nullptr; // 父分析栈

    // 名字空间子分析栈
    map<string, Stack*> spaces; // @函数栈  #匿名栈  具名栈
    list<tuple<string, Stack*>> uscps; // 正在使用的分析栈

    ElementStack stack;     // 当前分析栈
    
    ASTFunctionDefine* fndef = nullptr; // 当前正在定义的函数
    ASTTypeDefine*     tydef = nullptr; // 当前正在定义的类型
    map<string, TypeFunction*>  lambda_funcs; // lambda 函数参数表
    map<string, TypeFunction*>  func_param_lambdas; // 带lambda参数的函数

public:
    
    Stack(Stack*p=nullptr, Mod t=Mod::Anonymous);

    // 初始化基础分析栈
    void Initialize();
    
public:

    // 打印分析栈
    void print(const string &ind="");
    
    // 操作分析栈
    Element* put(const string &, Element*); // 放入当前栈
    Element* set(const string &, Element*, bool up = true);
    Element* find(const string &, bool up = true);
    TypeFunction* findLambda(const string &, bool up = true); // 查找 lambda
    // 查找名字空间
    Stack * use(const string &);
    // 查询函数是否定义，返回定义
    ASTFunctionDefine* findFunction(TypeFunction*);
    
    // 添加新用户定义函数
    void addFunction(TypeFunction*, ASTGroup*);
    void addFunction(ASTFunctionDefine*);
    void addBuiltinFunction(const string &); // 通过字符串添加内建函数

};

#undef Namespace


}
}
