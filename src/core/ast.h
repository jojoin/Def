#pragma once

/** 
 * Def �﷨������
 */


#include <string>
#include <vector>
#include <tuple>
#include <set>
#include <map>
#include <list>
#include <iostream>

#include "../global.h"

#include "./type.h"
#include "./error.h"

#include "../compile/gen.h"
#include "../parse/tokenizer.h"
#include "../util/str.h"

#include "llvm/IR/IRBuilder.h"

namespace def {
namespace core {
    
using namespace std;
using namespace def::util;
using namespace def::parse;
using namespace def::compile;


/**
 * Def ��������� 
 */
struct AST
{
    virtual llvm::Value* codegen(Gen &) { return nullptr;  };
    virtual void print(string pre="", string ind="") {};
    virtual def::core::Type* getType() {
        FATAL("cannot call getType() , this AST is not a <value> !")
    };
    // �Ƿ�Ϊ value ֵ�����˺��������͵�����������ȣ�
    virtual bool isValue() { return false; }; 
    virtual bool isCodegen() { return false; };
};


class ASTTypeDefine;
class ASTFunctionDefine;



#define AST_HEAD(N) \
struct AST##N : AST \
{ \
    virtual void print(string, string); \


#define AST_CODE_HEAD(N) \
AST_HEAD(N) \
    virtual llvm::Value* codegen(Gen &); \
    virtual def::core::Type* getType(); \
    virtual bool isCodegen() { return true; }; \


#define AST_VALUE_CODE_HEAD(N) \
AST_CODE_HEAD(N) \
    virtual bool isValue() { return true; }; \


// ��
AST_VALUE_CODE_HEAD(Group)
    vector<AST*> childs; // �б�
    void add(AST*a); // ����Ӿ�
};


// ����
AST_VALUE_CODE_HEAD(Constant)
    Type* type; // ��������
    string value; // ��������ֵ
    ASTConstant(Type*t, const string&v)
        : type(t)
        , value(v)
    {}
};


// ���ڴ�����
AST_VALUE_CODE_HEAD(Malloc)
    Type* type;   // ��������
    AST* len;   // ���볤��
    bool is_array = false; //�Ƿ�Ϊ����
    ASTMalloc(Type* t, AST* l)
        : len(l)
        , type(t)
    {}
};

// �����Ѷ���
AST_VALUE_CODE_HEAD(New)
    AST* obj;
    ASTNew(AST* c)
        : obj(c)
    {}
    
};


// ���ڴ��ͷ�
AST_VALUE_CODE_HEAD(Delete)
    AST* vptr;   // ���õ�ֵ
    ASTDelete(AST* v)
        : vptr(v)
    {}
};


// ���󿽱�
AST_VALUE_CODE_HEAD(Copy)
    AST* value;   // ������ֵ
    ASTCopy(AST* v)
        : value(v)
    {}
};


// ����ֵ
AST_VALUE_CODE_HEAD(Quote)
    Type* type;   // ��������
    AST* value;   // ���õ�ֵ
    ASTQuote(AST* v, Type* t)
        : value(v)
        , type(t)
    {}
};


// ����������ֵ
AST_VALUE_CODE_HEAD(Load)
    Type* type;   // ��������
    AST* value;   // ���õ�ֵ
    ASTLoad(AST* v, Type* t)
        : value(v)
        , type(t)
    {}
};

// �����ʼ��
AST_VALUE_CODE_HEAD(ArrayConstruct)
    TypeArray* type;   // ��������
    ASTArrayConstruct(TypeArray* t)
        : type(t)
    {}
};

// �����Ա����
AST_VALUE_CODE_HEAD(ArrayVisit)
    AST* index;    // ��Ԫ������
    AST* instance; // ����ʵ��
    ASTArrayVisit(AST*v=nullptr, AST* i=nullptr)
        : index(i)
        , instance(v)
    {}
};

// �����Ա��ֵ
AST_VALUE_CODE_HEAD(ArrayAssign)
    AST* index; // ��Ԫ������
    AST* instance; // ����ʵ��
    AST* value; // ��ֵ
    ASTArrayAssign(AST*m=nullptr, AST* i=nullptr, AST*c=nullptr)
        : index(i)
        , instance(m)
        , value(c)
    {}
};


// ��������ֵ
AST_VALUE_CODE_HEAD(Ret)
    AST* value;
    ASTRet(AST*v)
        : value(v)
    {}
};


// if ������֧
AST_CODE_HEAD(If)
    bool canphi=false; // IF��֧������һ�£��ɽ��� PHI �ڵ�
    AST* cond;
    AST* pthen=nullptr;
    AST* pelse=nullptr;
    ASTIf(AST*c)
        : cond(c)
    {}
    virtual bool isValue() { 
        if (pelse && pthen && 
            pthen->getType()->is(pelse->getType())) {
            return true;
        }
        return false;
    }; 
};


// while ѭ��
AST_CODE_HEAD(While)
    AST* cond;
    AST* body=nullptr;
    ASTWhile(AST*c)
        : cond(c)
    {}
    virtual bool isValue() { 
        return false;
    }; 
};


// ���Ͷ���
AST_HEAD(TypeDefine)
    TypeStruct* type;
    vector<ASTFunctionDefine*> members; // ��Ա����
    // bool checkMember(ASTFuntionDefine* fdef);
    // void addMember(ASTFuntionDefine* fdef);
};


// ����������
AST_HEAD(TypeRename)
    string name;
    Type* type;
    ASTTypeRename(Type*t=nullptr, const string & n="")
    : type(t)
    , name(n)
    {}
};


// �ⲿ��Ա��������
AST_HEAD(ExternalMemberFunctionDefine)
    TypeStruct* type;
    AST* defs;
    ASTExternalMemberFunctionDefine(TypeStruct*t=nullptr,AST*c=nullptr)
        : type(t)
        , defs(c)
    {}
};



// ���͹���
AST_VALUE_CODE_HEAD(TypeConstruct)
    TypeStruct* type;
    vector<AST*> childs; //
    AST *instance = nullptr; // �ѽ��ڱ�(��)������ڴ�
    bool bare = false; // �չ���
    ASTTypeConstruct(TypeStruct*t=nullptr, bool b=false)
        : type(t)
        , bare(b)
    {}
    void add(AST*);
};


// ����
AST_VALUE_CODE_HEAD(Variable)
    string name; // ����
    string unique_name; // ������ȫ��Ψһ����
    Type* type;  // ����
    AST* origin;  // ��Դ�ڵ�
    ASTVariable(const string&n, Type*t)
        : name(n)
        , type(t)
    {}
};


// ��������
AST_VALUE_CODE_HEAD(VariableDefine)
    string name;
    string unique_name; // ������ȫ��Ψһ����
    AST* value; // 
    ASTVariableDefine(const string &n = "", AST*v = nullptr)
        : name(n)
        , value(v)
    {
        unique_name = getUniqueName();
    }
    static string getUniqueName()
    {
        static int nameidx = 0;
        nameidx++;
        return "#" + Str::l2s(nameidx);
    }
};


// ������ֵ
AST_VALUE_CODE_HEAD(VariableAssign)
    string name;
    string unique_name; // ������ȫ��Ψһ����
    AST* value; // 
    ASTVariableAssign(const string &n = "", AST*v = nullptr)
        : name(n)
        , value(v)
    {}
};


// ��������
AST_HEAD(FuntionDeclare)
    TypeFunction* ftype; // ��������
    ASTFuntionDeclare(TypeFunction*ft)
        : ftype(ft)
    {}
};


// ��������
AST_HEAD(FunctionDefine)
    TypeFunction* ftype; // ��������
    ASTGroup* body; // ������
    ASTFunctionDefine* wrap = nullptr; // ��㺯��
    ASTTypeDefine*  belong = nullptr; // ������
    bool is_static_member  = true;    // �Ƿ�Ϊ��̬��Ա����
    bool is_construct  = false;    // �Ƿ�Ϊ���캯��
    set<string> cptmbr;        // ����ʹ�õ����Ա����
    map<string, tuple<Type*, string>> cptvar;  // �������������ı���
    ASTFunctionDefine(TypeFunction*ft, ASTGroup *bd=nullptr)
        : ftype(ft)
        , body(bd)
    {}
    string getWrapPrefix(); // ��ȡ��㺯��ǰ׺
    string getIdentify();   // ��ȡΨһ�ĺ�������
};

// ��������
AST_VALUE_CODE_HEAD(FunctionCall)
    ASTFunctionDefine* fndef; // ��������
    vector<AST*> params; // ʵ��ֵ��
    ASTFunctionCall(ASTFunctionDefine*fd=nullptr)
        : fndef(fd)
    {}
    void addparam(AST*);// ���ʵ��
};

// ��Ա��������
AST_VALUE_CODE_HEAD(MemberFunctionCall)
    ASTFunctionCall* call; // ��������
    AST* instance; // ��ʵ��
    ASTMemberFunctionCall(AST*v=nullptr, ASTFunctionCall*c=nullptr)
        : call(c)
        , instance(v)
    {}
};

// ���Ա����
AST_VALUE_CODE_HEAD(MemberVisit)
    size_t index; // ��Ԫ������
    AST* instance; // ��ʵ��
    ASTMemberVisit(AST*v=nullptr, size_t i=0)
        : index(i)
        , instance(v)
    {}
};

// ��Ա��ֵ
AST_VALUE_CODE_HEAD(MemberAssign)
    size_t index; // ��Ԫ������
    AST* instance; // ��ʵ��
    AST* value; // ��ֵ
    ASTMemberAssign(AST*m=nullptr, size_t i=0, AST*c=nullptr)
        : index(i)
        , instance(m)
        , value(c)
    {}
};


// ����ģ�嶨��
AST_HEAD(TemplateFuntionDefine)
    string name;
    vector<string> params;
    list<Tokenizer::Word> bodywords; // �����嵥�ʱ�
    ASTTemplateFuntionDefine()
    {}
    void addword(string);
};


// ��ģ�嶨��
AST_HEAD(TemplateTypeDefine)
    string name;
    vector<string> params;
    list<Tokenizer::Word> bodywords; // �����嵥�ʱ�
    ASTTemplateTypeDefine()
    {}
    void addword(string);
};


// let ���Ű�
AST_HEAD(Let)
    vector<string> head;
    vector<string> body;
    ASTLet()
    {}
};


// uvnnew
AST_HEAD(UVNnew)
    string key;
    string uvn;
    ASTUVNnew(const string & k, const string & n)
        : key(k)
        , uvn(n)
    {}
};


// uvndel
AST_HEAD(UVNdel)
    string key;
    ASTUVNdel(const string & k)
        : key(k)
    {}
};


// uvnclear
AST_HEAD(UVNclear)
    ASTUVNclear(){}
};


// ChildScope
AST_CODE_HEAD(ChildScope)
    string name;
    vector<AST*> childs;
    ASTChildScope(const string & n="")
    : name(n)
    {}
};

// Use Scpoe
AST_HEAD(UseScope)
    string name;
    ASTUseScope(const string & n)
        : name(n)
    {}
};

// Delete Scope
AST_HEAD(DeleteScope)
    string name;
    ASTDeleteScope(const string & n)
        : name(n)
    {}
};




#undef AST_HEAD



}
}

