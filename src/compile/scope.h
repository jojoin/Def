#pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"


#include <string>
#include <map>



namespace def{
namespace compile{

using namespace std;
using namespace llvm;

// 变量函数分析栈
class Scope
{
    public:
    
    Scope * parent; // 父栈
    map<string,Value*> stack; // 栈

    Scope(Scope *p=nullptr)
    : parent(p)
    {}


    public:
    // 当前域放置
    bool put(const string & n, Value*v)
    {
        auto it = stack.find(n);
        if(it != stack.end()){
            return false; // 已存在不能放入
        }
        stack[n] = v;
        return true;
    }
    // 替换，返回旧值
    Value* set(const string & n, Value*v)
    {
        auto it = stack.find(n);
        if(it != stack.end()){
            auto *old = it->second;
            stack[n] = v;
            return old;
        }
        // 查找父级
        if(parent){
            return parent->set(n, v);
        }
        // 不存在变量
        return nullptr;
    }
    // 获取
    Value* get(const string & n)
    {
        auto it = stack.find(n);
        if(it != stack.end()){
            return it->second;
        }
        // 查找父级
        if(parent){
            return parent->get(n);
        }
        // 不存在变量
        return nullptr;
    }

};


}
}
