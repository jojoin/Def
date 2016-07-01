#pragma once


/** 
 * Def ��������
 */


#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../global.h"

#include "./splitfix.h"
#include "../util/str.h"


// def ��ԭ�������б�
#define DEF_AOTM_TYPE_LIST(T) \
    T(Nil) \
    T(Bool) \
    T(Int) \
    T(Float) \
    T(Char) \
    T(String) \


//    T(Quote) /* ���� */ \




namespace def {
namespace core {
    
using namespace std;
using namespace def::util;


/**
 * Def �������� 
 */
struct Type
{
    virtual string str() = 0;    // �ַ�����ʾ
    virtual void set(Type*) {};  // ���� type
    virtual void add(Type*) {};  // ���� type
    virtual size_t len() { return 1; }; // type ����
    virtual Type* copy(const string& n="") { // ���� type
        return nullptr;
    };
    virtual bool is(Type*t) = 0; // �ж����������Ƿ����
    virtual bool isAtomType() = 0; // �Ƿ�Ϊԭ������
    virtual string getIdentify(bool strict=true) { // ���Ψһ��ʶ
        return str();
    }
// static function
    static map<string, Type*> types; // �����ԭ������
    static Type* get(const string &);

};


// ԭ������
#define ATOM_TYPE(N) \
struct Type##N : Type \
{ \
    virtual bool isAtomType(){ return true; } \
    virtual string str() { \
        return #N; \
    } \
    virtual bool is(Type*t){ /* ԭ�����Ͳ�������ʱ�ж� */ \
        return dynamic_cast<Type##N*>(t); \
    } \
};

#define AT(T) ATOM_TYPE(T)
DEF_AOTM_TYPE_LIST(AT)
#undef AT

#undef AOTM_TYPE



// ��չ����
#define NOATOM_TYPE(N) \
struct Type##N : Type \
{ \
    virtual bool isAtomType() { return false; }; 



#define TYPE_POND(T) \
    /* ָ�����ͳ� */ \
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






// ָ������
// �����ڲ�������ʵ��ʹ��
// ���ִӶ��Ϸ���Ķ���
NOATOM_TYPE(Pointer)
    Type* type = nullptr; // ����ֵ������
    TypePointer(Type*t)
        : type(t)
    {}
    virtual string str() {
        return getIdentify();
    }
    virtual string getIdentify(bool strict=true) { // ���Ψһ��ʶ
        return "*" + type->getIdentify(strict);
    }
    virtual bool is(Type*t){
        if (auto *ty = dynamic_cast<TypePointer*>(t)) {
            return type->is(ty->type); // ָ������һ��
        }
        return false;
    }
    // ���ͳ�
    TYPE_POND(Pointer)
};




// ��������
NOATOM_TYPE(Refer)
    // size_t len;
    Type* type = nullptr; // ����ֵ������
    TypeRefer(Type*t)
        : type(t)
    {}
    virtual bool is(Type*t){
        if (auto *ty = dynamic_cast<TypeRefer*>(t)) {
            return ty->type->is(type); // ��������һ��
        }
        return false;
    }
    virtual void set(Type* t) { // ���� type
        type = t;
    }
    virtual string str() {
        return getIdentify();
    }
    virtual string getIdentify(bool strict=true) { // ���Ψһ��ʶ
        return "~" + type->getIdentify(strict);
    }
    // ���ͳ�
    TYPE_POND(Refer)
};



// ��������
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
        // ���Ψһ��ʶ��strict ��ʾ��СҲ�������
        return "["
            + (strict ? Str::l2s(len) + "x" : "")
            + type->getIdentify(strict)
            + "]";
    }
    virtual void set(Type* t) { // ���� type
        type = t;
    }
    virtual bool is(Type*t){
        if (auto *ty = dynamic_cast<TypeArray*>(t)) {
            return ty->type->is(type); // ��������һ��
        }
        return false;
    }
    // ���ͳ�
    TYPE_POND(Array)
};


// ��չ����
#define EXTEND_TYPE(N,P) \
struct Type##N : P \
{ \
    virtual bool isAtomType() { return false; };


// �ṹ����
EXTEND_TYPE(Struct, Type)
    long idx = 0;
    static long auto_idx; // ����Ψһ��ʶ������
    string name; // ������
    vector<string> tabs; // �����ʶ��
    vector<Type*> types; // �����б�
    TypeStruct(const string&n)
        : name(n) {
        // cout << "TypeStruct(const string&n)"<< n << endl;
    }
    void increment() {
        idx = ++auto_idx;
    }
    virtual bool is(Type*t){ // ��չ����ֱ�ӶԱȵ�ַ
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
            return true; // Ԫ�ظ�����ȣ�ÿ��Ԫ�����
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
    virtual void add(Type* t, const string&n="") { // ���� type
        tabs.push_back(n);
        types.push_back(t);
    }
    virtual void front(Type* t, const string&n="") { // ���� type
        tabs.insert(tabs.begin(), n);
        types.insert(types.begin(), t);
    }
    virtual size_t len() {  // type ����
        return types.size();
    };
    virtual Type* copy(const string&n="") {       // ���� type
        TypeStruct* nts = new TypeStruct(n);
        nts->types = types; // �����ṹ
        return nts; // �����½�������
    };
    int elmpos(const string&n) {  // ��Ԫ��ƫ��
        int i = 0;
        for(auto &it : tabs) {
            if (it==n) {
                return i;
            }
            i++;
        }
        return -1;
    };
    Type* elmget(const string&n) {  // ��Ԫ��ƫ��
        int i = 0;
        for(auto &it : tabs) {
            if (it==n) {
                return types[i];
            }
            i++;
        }
        return nullptr;
    };
    virtual string getIdentify(bool strict=true) { // ���Ψһ��ʶ
        if(name == ""){ // �����ṹ
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


// ��������
EXTEND_TYPE(Function,TypeStruct)
    Type* ret; // ����ֵ����
    TypeFunction(const string&n, Type*t=nullptr)
        : TypeStruct(n)
        , ret(t)
    {}
    virtual bool is(Type*t){ // ��չ����ֱ�ӶԱȵ�ַ
        return !!( ((int)this)==((int)t) );
    }
    virtual void set(Type* t) { // ���÷���ֵ type
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
    virtual string getIdentify(bool strict=true) { // ���Ψһ��ʶ
        string identify(name);
        for(auto& it : types) {
            identify += DEF_SPLITFIX_FUNCARGV+it->getIdentify(strict);
        }
        return identify;
    }
    TypeFunction* clone() {  // ��¡
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

