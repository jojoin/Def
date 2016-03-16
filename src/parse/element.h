#pragma once

#include <string>
#include "../core/ast.h"
#include "./tokenizer.h"


namespace def {
namespace parse {

using namespace std;
using namespace def::core;


/**
 * 语法分析栈 唯一标示 元素
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

ELEMENT_HEAD(Variable, Type, type) // 变量
    AST* origin = nullptr;
};
ELEMENT_HEAD(Type, Type, type) // 类型名
};
ELEMENT_HEAD(Function, ASTFunctionDefine, fndef) // 函数名 
};
ELEMENT_HEAD(TemplateFuntion, ASTTemplateFuntionDefine, tpfdef) // 模板推断函数
};
struct ElementLet : Element // let 符号绑定
{
    vector<string> params; // 参数
    vector<Tokenizer::Word> bodywords; // 解析后的体
    virtual string str();
};
struct ElementGroup : Element // 组（函数重载）
{
    map<string, Element*> elms;
    void filter(string, vector<Element*>&);
    virtual string str();
};


#undef ELEMENT_HEAD

}
}
