#pragma once

#include <string>
#include "../core/ast.h"
#include "./tokenizer.h"


namespace def {
namespace parse {

using namespace std;
using namespace def::core;


/**
 * �﷨����ջ Ψһ��ʾ Ԫ��
 */

struct Element
{
    virtual string str() { return ""; };
};

#define ELEMENT_HEAD(T,V,v) \
struct Element##T : Element \
{ \
    V* v; \
    Element##T(V* n=nullptr) \
        : v(n) \
    {} \
    virtual string str(); \

ELEMENT_HEAD(Variable, Type, type) // ����
    AST* origin = nullptr;
    string unique_name = ""; // ȫ��Ψһ����
};
ELEMENT_HEAD(Type, Type, type) // ������
};
ELEMENT_HEAD(Function, ASTFunctionDefine, fndef) // ������ 
};
ELEMENT_HEAD(TemplateFuntion, ASTTemplateFuntionDefine, tpfdef) // ����ģ��
};
ELEMENT_HEAD(TemplateType, ASTTemplateTypeDefine, tptydef) // ��ģ��
};
struct ElementLet : Element // let ���Ű�
{
    vector<string> params; // ����
    vector<Tokenizer::Word> bodywords; // ���������
    virtual string str();
};
struct ElementGroup : Element // �飨�������أ�
{
    map<string, Element*> elms;
    void filter(string, vector<Element*>&);
    virtual string str();
};


#undef ELEMENT_HEAD

}
}
