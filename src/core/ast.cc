/**
 * Ast
 */


#include "./ast.h"
#include "../parse/stack.h"
#include "../parse/service.h"
#include "./error.h"


using namespace def::core;
using namespace def::parse;


#define IND    "  "
#define INDLIN "��"
#define INDCON "��"
#define INDEND "�t" // �������t



#define FUNC_HEAD_GETTYPE(T) \
def::core::Type* AST##T::getType() \
{ \


#define FUNC_HEAD_PRINT(T) \
void AST##T::print(string pre, string ind) \
{ \
    cout << pre;


#define PRINT_ONE_CHILD(T) \
    T->print(ind+INDEND, ind+IND); \


#define PRINT_CHILDS(T) \
    int len = T.size(); \
    for (int i = 0; i < len; ++i) { \
        if (i + 1 == len){ \
            T[i]->print(ind + INDEND, ind + IND); \
        } else { \
            T[i]->print(ind + INDCON, ind + INDLIN); \
        } \
    } \


#define PRINT_CHILD_CHECK(T) \
    if (T) { \
        if (auto *gr = dynamic_cast<ASTGroup*>(T)) { \
            PRINT_CHILDS(gr->childs) \
        } else { \
            PRINT_ONE_CHILD(T) \
        } \
    } \




/**
 * Let
 */
FUNC_HEAD_PRINT(Let)
    cout << "let:  ";
    for(auto &h : head){
        cout << h << " ";
    }
    cout << " =>  ";
    for(auto &b : body){
        cout << b << " ";
    }
    cout << endl;
}


/**
 * Ret
 */
FUNC_HEAD_PRINT(Ret)
    cout << "return";
    if (value) {
        cout << ":" << endl;
        PRINT_ONE_CHILD(value)
    } else {
        cout << " void" << endl;
    }
}
FUNC_HEAD_GETTYPE(Ret)
    return value ? value->getType() : Type::get("Nil");
}



/**
 * Group
 */
FUNC_HEAD_PRINT(Group)
    cout << "group " << childs.size() << ": " << endl;
    PRINT_CHILDS(childs)
}
FUNC_HEAD_GETTYPE(Group)
    return childs.back()->getType();
}
void ASTGroup::add(AST*a)
{
    childs.push_back(a);
}


/**
 * Constant
 */
FUNC_HEAD_PRINT(Constant)
    cout << "# " << value << endl;
}
FUNC_HEAD_GETTYPE(Constant)
    return type;
}


/**
 * Malloc
 */
FUNC_HEAD_PRINT(Malloc)
    cout << "malloc <" << type->getIdentify() + "> length: " << endl;
    PRINT_ONE_CHILD(len)
}
FUNC_HEAD_GETTYPE(Malloc)
    if (is_array) {
        return TypePointer::get(TypeArray::get(type));
    } else {
        return TypePointer::get(type);
    }
}


/**
 * New
 */
FUNC_HEAD_PRINT(New)
    cout << "new: " << endl;
    PRINT_ONE_CHILD(obj)
}
FUNC_HEAD_GETTYPE(New)
    return obj->getType();
}


/**
 * Delete
 */
FUNC_HEAD_PRINT(Delete)
    cout << "delete: " << endl;
    PRINT_ONE_CHILD(vptr)
}
FUNC_HEAD_GETTYPE(Delete)
    return Type::get("Nil");
}


/**
 * Copy
 */
FUNC_HEAD_PRINT(Copy)
    cout << "Copy: " << endl;
    PRINT_ONE_CHILD(value)
}
FUNC_HEAD_GETTYPE(Copy)
    return value->getType();
}


/**
 * Quote
 */
FUNC_HEAD_PRINT(Quote)
    cout << "quote: " << endl;
    PRINT_ONE_CHILD(value)
}
FUNC_HEAD_GETTYPE(Quote)
    return type;
}


/**
 * Load
 */
FUNC_HEAD_PRINT(Load)
    cout << "load: " << endl;
    PRINT_ONE_CHILD(value)
}
FUNC_HEAD_GETTYPE(Load)
    return ((TypeRefer*)type)->type;
}


/**
 * ArrayConstruct
 */
FUNC_HEAD_PRINT(ArrayConstruct)
    cout << "array construct: " << type->str() << endl;
}
FUNC_HEAD_GETTYPE(ArrayConstruct)
    return type;
}


/**
 * ArrayVisit
 */
FUNC_HEAD_PRINT(ArrayVisit)
    string indent = ind;
    ind = indent + INDLIN;
    cout << "array visit: " << instance->getType()->str() << endl;
    // instance
    cout << indent+INDCON << "instance: " << endl;
    PRINT_ONE_CHILD(instance)
    // index
    ind = indent + IND;
    cout << indent+INDEND << "index: " << endl;
    PRINT_CHILD_CHECK(index)
}
FUNC_HEAD_GETTYPE(ArrayVisit)
    if (auto scty = Service::validTypeArray(instance)) {
        return scty->type;
    }
    FATAL("cannot getType() , ArrayVisit instance is not a array type !")
}


/**
 * ArrayAssign
 */
FUNC_HEAD_PRINT(ArrayAssign)
    string indent = ind;
    ind = indent + INDLIN;
    cout << "array assign: " << instance->getType()->str() << endl;
    // instance
    cout << indent+INDCON << "instance: " << endl;
    PRINT_ONE_CHILD(instance)
    // index
    cout << indent+INDCON << "index: " << endl;
    PRINT_CHILD_CHECK(index)
    // value
    ind = indent + IND;
    cout << indent+INDEND << "value: " << endl;
    PRINT_CHILD_CHECK(value)
}
FUNC_HEAD_GETTYPE(ArrayAssign)
    return value->getType();
}



/**
 * If
 */
FUNC_HEAD_PRINT(If)
    string indent = ind;
    ind = indent + INDLIN;
    cout << "if: " << endl;
    // cond
    cout << indent+INDCON << "cond: " << endl;
    PRINT_ONE_CHILD(cond)
    // then
    cout << indent+INDCON << "then: " << endl;
    PRINT_CHILD_CHECK(pthen)
    // else
    ind = indent + IND;
    cout << indent+INDEND << "else: " << endl;
    PRINT_CHILD_CHECK(pelse)
}
FUNC_HEAD_GETTYPE(If)
    Type* t1 = pthen->getType();
    Type* t2 = pelse->getType();
    if (t1->is(t2)) {
        return t1;
    }
    return Type::get("Nil");
}


/**
 * While
 */
FUNC_HEAD_PRINT(While)
    string indent = ind;
    ind = indent + INDLIN;
    cout << "while: " << endl;
    // cond
    cout << indent+INDCON << "cond: " << endl;
    PRINT_ONE_CHILD(cond)
    // body
    ind = indent + IND;
    cout << indent+INDEND << "body: " << endl;
    PRINT_CHILD_CHECK(body)
}
FUNC_HEAD_GETTYPE(While)
    return Type::get("Nil");// body->getType();
}


/**
 * FunctionCall
 */
void ASTFunctionCall::addparam(AST*a)
{
    params.push_back(a);
}
FUNC_HEAD_PRINT(FunctionCall)
    cout << "function call: " << fndef->ftype->getIdentify() << endl;
    PRINT_CHILDS(params)
}
FUNC_HEAD_GETTYPE(FunctionCall)
    return fndef->ftype->ret;
}


/**
 * MemberVisit
 */
FUNC_HEAD_PRINT(MemberVisit)
    cout << "member visit: " << index << endl;
    PRINT_ONE_CHILD(instance)
}
FUNC_HEAD_GETTYPE(MemberVisit)
    if (auto scty = Service::validTypeStruct(instance)) {
        return scty->types[index];
    }
    FATAL("cannot getType() , MemberVisit instance is not a class type !")
}


/**
 * MemberAssign
 */
FUNC_HEAD_PRINT(MemberAssign)
    //PRINT_ONE_CHILD(instance)
    //PRINT_ONE_CHILD(value)
    string indent = ind;
    ind = indent + INDLIN;
    cout << "member assign: " << index << endl;
    // instance
    cout << indent+INDCON << "instance: " << endl;
    PRINT_ONE_CHILD(instance)
    // value
    ind = indent + IND;
    cout << indent+INDEND << "value: " << endl;
    PRINT_CHILD_CHECK(value)
}
FUNC_HEAD_GETTYPE(MemberAssign)
    return value->getType();
}


/**
 * MemberFunctionCall
 */
FUNC_HEAD_PRINT(MemberFunctionCall)
    if (!instance) { // ��̬��Ա��������
        cout << "static member function call: " << endl;
        PRINT_ONE_CHILD(call)
        return;
    }
    cout << "member function call: " << endl;
    string indent = ind;
    ind = indent + INDLIN;
    // instance
    cout << indent+INDCON << "instance: " << endl;
    PRINT_ONE_CHILD(instance)
    // call
    ind = indent + IND;
    cout << indent+INDEND << "call: " << endl;
    PRINT_CHILD_CHECK(call)
}
FUNC_HEAD_GETTYPE(MemberFunctionCall)
    // ����ǹ��캯��
    if (call->fndef->is_construct) {
        return instance->getType();
    }
    return call->getType();
}


/**
 * ExternalMemberFunctionDefine
 */
FUNC_HEAD_PRINT(ExternalMemberFunctionDefine)
    cout << "external member function define: " 
         << type->getIdentify()
         << endl;
    PRINT_CHILD_CHECK(defs)
}




/**
 * Variable
 */
FUNC_HEAD_PRINT(Variable)
    cout << "$" << name << ": " << type->getIdentify() << endl;
}
FUNC_HEAD_GETTYPE(Variable)
    return type;
}

/**
 * VariableDefine
 */
FUNC_HEAD_PRINT(VariableDefine)
    cout << "variable define $" << name << ":" << endl;
    PRINT_ONE_CHILD(value)
}
FUNC_HEAD_GETTYPE(VariableDefine)
    return value->getType();
}


/**
 * VariableAssign
 */
FUNC_HEAD_PRINT(VariableAssign)
    cout << "variable assign $" << name << ":" << endl;
    PRINT_ONE_CHILD(value)
}
FUNC_HEAD_GETTYPE(VariableAssign)
    return value->getType();
}


/**
 * TypeDefine
 */
FUNC_HEAD_PRINT(TypeDefine)
    auto *sty = dynamic_cast<TypeStruct*>(type);
    cout << "type define: " << sty->getIdentify() << "{";
    bool fx = false;
    for (auto *p : sty->types) {
        if(fx) cout << ",";
        fx = true;
        cout << p->getIdentify();
    }
    cout << "}" << endl;
    // Stack* stk = (Stack*)stack;
    // stk->print();
    PRINT_CHILDS(members)
}


/**
 * TypeRename
 */
FUNC_HEAD_PRINT(TypeRename)
    cout << "type rename: " << name << " = " << type->getIdentify() << endl;
}


/**
 * TypeConstruct
 */
void ASTTypeConstruct::add(AST*a) { // ��Ӳ���
    childs.push_back(a);
}
FUNC_HEAD_PRINT(TypeConstruct)
    cout << "type construct: " << type->getIdentify() << endl;
    PRINT_CHILDS(childs)
}
FUNC_HEAD_GETTYPE(TypeConstruct)
    if( instance ){ // �ѷ����ڴ�
        return instance->getType();
    }
    return type;
}

/**
 * FuntionDeclare
 */
FUNC_HEAD_PRINT(FuntionDeclare)
    cout << "function declare: " << ftype->getIdentify() << endl;
}


/**
 * FuntionDefine
 */
FUNC_HEAD_PRINT(FunctionDefine)
    cout << "function define: " << ftype->getIdentify() << endl;
    PRINT_CHILDS(body->childs)
}
// ��ȡ��㺯��ǰ׺
string ASTFunctionDefine::getWrapPrefix()
{
    // cout << "ASTFuntionDefine::getWrapPrefix()" << endl;
    if (wrap) {
        return wrap->getWrapPrefix() + wrap->ftype->getIdentify() + DEF_SPLITFIX_FUNCWRAP;
    } else {
        return "";
    }
}
// ��ȡΨһ�ĺ�������
string ASTFunctionDefine::getIdentify()
{
    string clas("");
    if (belong) {
        clas = belong->type->getIdentify() + DEF_SPLITFIX_MEMFUNC_ISTC;
    }

    return clas + getWrapPrefix() + ftype->getIdentify();
}

/**
 * TemplateFuntionDefine
 */
void ASTTemplateFuntionDefine::addword(string n)
{
    params.push_back(n);
}
FUNC_HEAD_PRINT(TemplateFuntionDefine)
    cout << "template function define: " 
        << name << "(";
    bool fx = false;
    for (auto &it : params) {
        if(fx) cout << " ";
        fx = true;
        cout << it;
    }
    cout << ")" << endl;
    cout << ind+IND;
    for (auto &it : bodywords) {
        cout << it.str() << " ";
    }
    cout << endl;
}


/**
 * TemplateTypeDefine
 */
void ASTTemplateTypeDefine::addword(string n)
{
    params.push_back(n);
}
FUNC_HEAD_PRINT(TemplateTypeDefine)
    cout << "template type define: " 
        << name << "(";
    bool fx = false;
    for (auto &it : params) {
        if(fx) cout << " ";
        fx = true;
        cout << it;
    }
    cout << ")" << endl;
    cout << ind+IND;
    for (auto &it : bodywords) {
        cout << it.str() << " ";
    }
    cout << endl;
}



// uvnnew
FUNC_HEAD_PRINT(UVNnew)
    cout << "unv define: "<< key <<" = " << uvn << endl;
}
// uvndel
FUNC_HEAD_PRINT(UVNdel)
    cout << "unv delete: '"<< key << endl;
}
// uvnclear
FUNC_HEAD_PRINT(UVNclear)
    cout << "unv clear " << endl;
}


/**
 * ChildScope
 */
FUNC_HEAD_PRINT(ChildScope)
    cout << "child scope, len = " << childs.size() << " : " << endl;
    PRINT_CHILDS(childs)
}
FUNC_HEAD_GETTYPE(ChildScope)
    return childs.back()->getType();
}