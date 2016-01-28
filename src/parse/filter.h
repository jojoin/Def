#pragma once
/**
 * 栈筛选器
 */

#include <vector>
#include <set>
#include <string>

#include "./element.h"
#include "./stack.h"


namespace def {
namespace parse {

using namespace std;



// 操作符绑定筛选器
struct filterLet
{
    map<string, ElementLet*> lets;
    ElementLet* unique  = nullptr; // 唯一匹配
    filterLet(Stack*, const string &);
    // 筛选函数组
    void initElementGroup(Stack*, const string & );
    // 筛选并匹配 返回匹配数量
    size_t size();
    // 筛选并匹配 返回匹配数量
    size_t match(const string &);
};



// 函数筛选匹配
struct filterFunction
{
    vector<ElementFunction*> funcs;
    ASTFunctionDefine* unique  = nullptr; // 唯一匹配
    // up = 是否向上查找准备栈
    filterFunction(Stack*, const string &, bool up=true);
    // 筛选函数组
    void initElementGroup(Stack*, const string &);
    // 筛选并匹配 返回匹配数量
    size_t size();
    // 筛选并匹配 返回匹配数量
    size_t match(TypeFunction*);


};



}
}