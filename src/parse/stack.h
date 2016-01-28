#pragma once


#include <vector>
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
public:
    
    Stack(Stack*p=nullptr);

    // 初始化基础分析栈
    void Initialize();


    
public:

    // 打印分析栈
    void print();
    
    // 操作分析栈
    Element* put(const string &, Element*); // 放入当前栈
    Element* set(const string &, Element*, bool up = true);
    Element* find(const string &, bool up = true);
    // 查询函数是否定义，返回定义
    ASTFunctionDefine* findFunction(TypeFunction*);
    
    // 添加新用户定义函数
    void addFunction(TypeFunction*, ASTGroup*);
    void addFunction(ASTFunctionDefine*);
    void addBuiltinFunction(const string &); // 通过字符串添加内建函数

public:
    
    Stack* parent = nullptr; // 父分析栈

    ElementStack stack;     // 当前分析栈
    
    ASTFunctionDefine* fundef = nullptr; // 当前正在定义的函数
    ASTTypeDefine*    tydef  = nullptr; // 当前正在定义的类型


};

#undef Namespace


}
}
