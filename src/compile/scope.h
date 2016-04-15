#pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"


#include <string>
#include <map>



namespace def{
namespace compile{

using namespace std;
using namespace llvm;

// ������������ջ
class Scope
{
    public:
    
    Scope * parent; // ��ջ
    map<string,Value*> stack; // ջ

    Scope(Scope *p=nullptr)
    : parent(p)
    {}


    public:
    // ��ǰ�����
    bool put(const string & n, Value*v)
    {
        auto it = stack.find(n);
        if(it != stack.end()){
            return false; // �Ѵ��ڲ��ܷ���
        }
        stack[n] = v;
        return true;
    }
    // �滻�����ؾ�ֵ
    Value* set(const string & n, Value*v)
    {
        auto it = stack.find(n);
        if(it != stack.end()){
            auto *old = it->second;
            stack[n] = v;
            return old;
        }
        // ���Ҹ���
        if(parent){
            return parent->set(n, v);
        }
        // �����ڱ���
        return nullptr;
    }
    // ��ȡ
    Value* get(const string & n)
    {
        auto it = stack.find(n);
        if(it != stack.end()){
            return it->second;
        }
        // ���Ҹ���
        if(parent){
            return parent->get(n);
        }
        // �����ڱ���
        return nullptr;
    }

};


}
}
