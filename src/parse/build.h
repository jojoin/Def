#pragma once

/** 
 * Def �﷨������
 */


#include <string>
#include <vector>
#include <map>

#include "../global.h"
#include "../util/str.h"

#include "../core/builtin.h"
#include "../core/type.h"
#include "../core/ast.h"
#include "./tokenizer.h"
#include "./service.h"
#include "./stack.h"


namespace def {
namespace parse {
    
using namespace std;
using namespace def::core;
using namespace def::util;


/**
 * Def ast ������
 */
class Build : public Service
{
    
public:

    // ����������
    ASTGroup* createAST();

protected:
    
    Build(Tokenizer * t);
    
protected:
    
    list<AST*> prepare_builds; // ����Ľڵ�

    // AST* cur_fun_ret = nullptr; // ��ǰ��������ֵ�ڵ�

    void prepareBuild(AST*);
    void prepareBuild(const list<AST*> &);

    // ������������ cache ����ȥ����Ľڵ���
    AST* build(bool spread=true); // spread = ����� let ��
    // ���� Group
    ASTGroup* buildGroup();
    // ��������
    // AST* buildFunctionCall(const string &, ElementGroup*, bool istpf=false);
    // ��������ģ�����ģ��
    AST* buildTemplateFuntion(const string &, ElementTemplateFuntion*);
    AST* buildTemplateType(const string &, ElementTemplateType*);
    // ���Ķ��崦��
    AST* buildCoreDefine(const string &);
    // ���������Ա����
    AST* buildVaribale(Element*, const string &n="");
    // �������캯������
    AST* buildConstruct(TypeStruct*, AST* v=nullptr);
    AST* buildMacro(ElementLet*, const string &);
    AST* buildChildScope();

protected:

    // let ���Ű�չ��
    // AST* spreadLetBind(list<Tokenizer::Word>*pwds=nullptr);
    AST* buildOperatorBind();
    bool forecastOperatorBind(); // Ԥ���Ƿ���Ҫ�⿪���Ű�
    list<Tokenizer::Word> spreadOperatorBind(list<Tokenizer::Word>*pwds=nullptr);
    
protected:

    // ȫ��Ψһ����
    size_t uq_var_autoincrement = 0;
    map<string, string> uq_var_names;
    Tokenizer::Word getUniqueVariableName(const string &);

protected:

    // �����������ã�up=�Ƿ����ϲ���
    ASTFunctionCall* _functionCall(const string &, Stack*, bool up=true);
    // �Ӻ���ͷ���������壩�����������ͣ�declare=�Ƿ�Ϊ������ʽ
    TypeFunction* _functionType(bool declare=false);
    // �Զ���ӷ���ֵ
    Type* _autoAddFuncRet(ASTFunctionDefine*);
    // ����ģ��õ��µ�����
    TypeStruct* _templateType(ASTTemplateTypeDefine*);

    

// ���ܺ���
protected:
    // ���浥�ʶΣ����������ڲ��������ݣ�
    void cacheWordSegment(list<Tokenizer::Word>&, bool pure=true);
    // ������ͱ�ע
    Type* expectTypeState();
    // �����Ч�ı�ʾ������
    Tokenizer::Word expectIdName(const string &);
    // �ò���չ������
    // void expandMacro(list<Tokenizer::Word>, map<string, Tokenizer::Word>);

protected:

    /** def ���Ķ����б� **/
#define T(N) AST* build_##N();
    BUILTIN_DEFINES_LIST(T)
#undef T

    
};


}
}

