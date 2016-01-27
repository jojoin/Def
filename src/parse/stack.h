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

// 操作符绑定筛选器
struct filterLet
{
    map<string, ElementLet*> lets;
    ElementLet* unique  = nullptr; // 唯一匹配
    filterLet(Stack*stk, const string & name)
    {
        while (stk) { // 循环检查
            initElementGroup(stk, name);
            stk = stk->parent;
        }
    }
    // 筛选函数组
    void initElementGroup(Stack*stk, const string & name)
    {
        int len = name.size();
        for (auto &p : stk->stack) {
            if (auto *let = dynamic_cast<ElementLet*>(p.second)) {
                if (name == p.first.substr(0,len)) {
                    if (lets.end()==lets.find(name)) {
                        lets[p.first] = let;
                    }
                    if (name == p.first) {
                        if (!unique) unique = let;
                    }
                }
            }
        }
    }
    // 筛选并匹配 返回匹配数量
    size_t size()
    {
        return lets.size();
    }
    // 筛选并匹配 返回匹配数量
    size_t match(const string & name)
    {
        unique = nullptr; // 复位
        map<string, ElementLet*> new_lets;
        int len = name.size();
        for (auto &p : lets) {
            if (name == p.first.substr(0, len)) {
                if (name == p.first) {
                    if (!unique) unique = p.second;
                }
                new_lets[p.first] = p.second;
            }
        }
        lets = new_lets;
        return new_lets.size();
    }
};

// 函数筛选匹配
struct filterFunction
{
    vector<ElementFunction*> funcs;
    ASTFunctionDefine* unique  = nullptr; // 唯一匹配
    // up = 是否向上查找准备栈
    filterFunction(Stack*stk, const string & fname, bool up=true)
    {
        if(!up){
            initElementGroup(stk, fname);
        } else {
            while (stk) { // 循环检查
                initElementGroup(stk, fname);
                stk = stk->parent;
            }
        }
    }
    // 筛选函数组
    void initElementGroup(Stack*stk, const string & fname)
    {
        if (auto *fngr = dynamic_cast<ElementGroup*>(stk->find(fname, false))) {
            for(auto &p: fngr->elms){
                if (auto fun=dynamic_cast<ElementFunction*>(p.second)) {
                    funcs.push_back(fun);
                }
            }
        }
    }
    // 筛选并匹配 返回匹配数量
    size_t size()
    {
        return funcs.size();
    }
    // 筛选并匹配 返回匹配数量
    size_t match(TypeFunction* fty) 
    {
        unique = nullptr; // 复位
        vector<ElementFunction*> new_funcs;
        string tmpname = fty->getIdentify();
        int tmplen = tmpname.size();
        for (auto &p : funcs) {
            string mname = p->fndef->ftype->getIdentify();
            if (tmpname == mname.substr(0, tmplen)) {
                if (tmpname==mname) {
                    if(!unique) unique = p->fndef;
                }
                new_funcs.push_back(p);
            }
        }
        funcs = new_funcs;
        return new_funcs.size();
    }


};

}
}
