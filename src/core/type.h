#pragma once


/** 
 * Def 对象类型
 */


#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../global.h"

#include "./splitfix.h"
#include "../util/str.h"


// def 的原子类型列表
#define DEF_AOTM_TYPE_LIST(T) \
    T(Nil) \
    T(Bool) \
    T(Int) \
    T(Float) \
    T(Char) \
    T(String) \


//    T(Quote) /* 引用 */ \




namespace def {
namespace core {
    
using namespace std;
using namespace def::util;


/**
 * Def 对象类型 
 */
struct Type
{
    virtual string str() = 0;    // 字符串表示
    virtual void set(Type*) {};  // 设置 type
    virtual void add(Type*) {};  // 增加 type
    virtual size_t len() { return 1; }; // type 长度
    virtual Type* copy(const string& n="") { // 拷贝 type
        return nullptr;
    };
    virtual bool is(Type*t) = 0; // 判断两个对象是否相等
    virtual string getIdentify(bool strict=true) { // 获得唯一标识
        return str();
    }
// static function
    static map<string, Type*> types; // 缓存的原子类型
    static Type* get(const string &);

};


// 原子类型
#define AOTM_TYPE(N) \
struct Type##N : Type \
{ \
    virtual string str() { \
        return #N; \
    } \
    virtual bool is(Type*t){ /* 原子类型采用运行时判断 */ \
        return dynamic_cast<Type##N*>(t); \
    } \
};

#define AT(T) AOTM_TYPE(T)
DEF_AOTM_TYPE_LIST(AT)
#undef AT

#undef AOTM_TYPE


// 引用类型
struct TypeRefer : Type
{
    // size_t len;
    Type* type = nullptr; // 引用值的类型
    TypeRefer(Type*t)
        : type(t)
    {}
    virtual bool is(Type*t){
        if (auto *ty = dynamic_cast<TypeRefer*>(t)) {
            return ty->type->is(type); // 引用类型一致
        }
        return false;
    }
    virtual void set(Type* t) { // 增加 type
        type = t;
    }
    virtual string str() {
        return getIdentify();
    }
    virtual string getIdentify(bool strict=true) { // 获得唯一标识
        return "~" + type->getIdentify(strict);
    }
};



// 数组类型
struct TypeArray : Type
{
    size_t len;
    Type* type;
    TypeArray(Type*t, size_t l=0)
        : type(t)
        , len(l)
    {}
    virtual string str() {
        return getIdentify();
    }
    virtual string getIdentify(bool strict=true) { 
        // 获得唯一标识，strict 表示大小也必须相等
        return "["
            + (strict ? Str::l2s(len) + "*" : "")
            + type->getIdentify(strict)
            + "]";
    }
    virtual void set(Type* t) { // 增加 type
        type = t;
    }
    virtual bool is(Type*t){
        if (auto *ty = dynamic_cast<TypeArray*>(t)) {
            return ty->type->is(type); // 数组类型一致
        }
        return false;
    }
};



// 扩展类型
#define EXTEND_TYPE(N,P) \
struct Type##N : P \
{ \
    virtual bool is(Type*t){ /* 扩展类型直接对比地址 */ \
        return !!( ((int)this)==((int)t) ); \
    }


// 结构类型
EXTEND_TYPE(Struct, Type)
    long idx = 0;
    static long auto_idx; // 自增唯一标识！！！
    string name; // 类名称
    vector<string> tabs; // 子类标识符
    vector<Type*> types; // 子类列表
    TypeStruct(const string&n)
        : name(n) {
        // cout << "TypeStruct(const string&n)"<< n << endl;
    }
    void increment() {
        idx = ++auto_idx;
    }
    virtual string str() {
        string s = name+"{";
        bool f = true;
        for(auto& it : types) {
            if (f) {
                f = false;
            } else {
                s += ",";
            }
            s += it->str();
        }
        return s+"}";
    }
    virtual void add(Type* t, const string&n="") { // 增加 type
        tabs.push_back(n);
        types.push_back(t);
    }
    virtual size_t len() {  // type 长度
        return types.size();
    };
    virtual Type* copy(const string&n="") {       // 拷贝 type
        TypeStruct* nts = new TypeStruct(n);
        nts->types = types; // 拷贝结构
        return nts; // 返回新建的类型
    };
    int elmpos(const string&n) {  // 子元素偏移
        int i = 0;
        for(auto &it : tabs) {
            if (it==n) {
                return i;
            }
            i++;
        }
        return -1;
    };
    Type* elmget(const string&n) {  // 子元素偏移
        int i = 0;
        for(auto &it : tabs) {
            if (it==n) {
                return types[i];
            }
            i++;
        }
        return nullptr;
    };
    virtual string getIdentify(bool strict=true) { // 获得唯一标识
        if (idx == 0) {
            return name;
        }
        return name + "." + Str::l2s(idx);
    }

};


// 函数类型
EXTEND_TYPE(Function,TypeStruct)
    Type* ret; // 返回值类型
    TypeFunction(const string&n, Type*t=nullptr)
        : TypeStruct(n)
        , ret(t)
    {}
    virtual void set(Type* t) { // 设置返回值 type
        ret = t;
    }
    virtual string str() {
        string s(name);
        if (ret) {
            s += ": " + ret->getIdentify();
        }
        s += "(";
        bool fx = false;
        for(auto& it : types) {
            if (fx) s += ",";
            fx = true;
            s += it->getIdentify();
        }
        return s+")";
    }
    virtual string getIdentify(bool strict=true) { // 获得唯一标识
        string identify(name);
        for(auto& it : types) {
            identify += DEF_SPLITFIX_FUNCARGV+it->getIdentify(strict);
        }
        return identify;
    }
};



#undef EXTEND_TYPE



}
}

