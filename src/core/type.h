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
    virtual bool isAtomType() = 0; // 是否为原子类型
    virtual string getIdentify(bool strict=true) { // 获得唯一标识
        return str();
    }
// static function
    static map<string, Type*> types; // 缓存的原子类型
    static Type* get(const string &);

};


// 原子类型
#define ATOM_TYPE(N) \
struct Type##N : Type \
{ \
    virtual bool isAtomType(){ return true; } \
    virtual string str() { \
        return #N; \
    } \
    virtual bool is(Type*t){ /* 原子类型采用运行时判断 */ \
        return dynamic_cast<Type##N*>(t); \
    } \
};

#define AT(T) ATOM_TYPE(T)
DEF_AOTM_TYPE_LIST(AT)
#undef AT

#undef AOTM_TYPE



// 扩展类型
#define NOATOM_TYPE(N) \
struct Type##N : Type \
{ \
    virtual bool isAtomType() { return false; }; 



#define TYPE_POND(T) \
    /* 指针类型池 */ \
    static map<int, Type##T*> typtrs; \
    static Type##T* get(Type*t) { \
        int adr = (int)t; \
        auto it = typtrs.find(adr); \
        if (it != typtrs.end()) { \
            return it->second; \
        } else { \
            auto tar = new Type##T(t); \
            typtrs.insert(pair<int,Type##T*>(adr,tar)); \
            return tar; \
        } \
    } \






// 指针类型
// 仅供内部编译器实现使用
// 表现从堆上分配的对象
NOATOM_TYPE(Pointer)
    Type* type = nullptr; // 引用值的类型
    TypePointer(Type*t)
        : type(t)
    {}
    virtual string str() {
        return getIdentify();
    }
    virtual string getIdentify(bool strict=true) { // 获得唯一标识
        return "*" + type->getIdentify(strict);
    }
    virtual bool is(Type*t){
        if (auto *ty = dynamic_cast<TypePointer*>(t)) {
            return type->is(ty->type); // 指向类型一致
        }
        return false;
    }
    // 类型池
    TYPE_POND(Pointer)
};




// 引用类型
NOATOM_TYPE(Refer)
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
    // 类型池
    TYPE_POND(Refer)
};



// 数组类型
NOATOM_TYPE(Array)
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
            + (strict ? Str::l2s(len) + "x" : "")
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
    // 类型池
    TYPE_POND(Array)
};


// 扩展类型
#define EXTEND_TYPE(N,P) \
struct Type##N : P \
{ \
    virtual bool isAtomType() { return false; };


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
    virtual bool is(Type*t){ // 扩展类型直接对比地址
        if(name!=""){
            return !!( ((int)this)==((int)t) );
        }
        if(auto*sctty=dynamic_cast<TypeStruct*>(t)){
            size_t len = types.size();
            size_t len1 = sctty->types.size();
            if(len!=len1){
                return false;
            }
            while(len--){
                if(!types[len]->is(sctty->types[len])){
                    return false;
                }
            }
            return true; // 元素个数相等，每个元素相等
        }
        return false;
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
    virtual void front(Type* t, const string&n="") { // 增加 type
        tabs.insert(tabs.begin(), n);
        types.insert(types.begin(), t);
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
        if(name == ""){ // 匿名结构
            string idf = "";
            for(auto t : types){
                idf += "." + t->getIdentify();
            }
            return idf;
        }
        if(idx == 0){
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
    virtual bool is(Type*t){ // 扩展类型直接对比地址
        return !!( ((int)this)==((int)t) );
    }
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
    TypeFunction* clone() {  // 克隆
        auto *clnew = new TypeFunction("", ret);
        size_t len = types.size();
        for(int i = 0; i < len; i++){
            clnew->tabs.push_back(tabs[i]);
            clnew->types.push_back(types[i]);
        }
        return clnew;
    };
};


#undef EXTEND_TYPE



}
}

