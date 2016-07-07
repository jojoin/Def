#pragma once


#include <vector>
#include <tuple>
#include <list>
#include <set>
#include <string>

#include "./element.h"


namespace def {
namespace parse {

using namespace std;


#define ElementStack map<string, Element*>


/**
 * Def block ������
 */
class Stack
{
public:

    enum class Mod
    {
        Namespace,   //   ���ֿռ�
        Class,       // % ����ջ
        Function,    // @ ����ջ
        Anonymous,   // # ����ջ
    } mod = Mod::Anonymous;
    
    Stack* parent = nullptr; // ������ջ

    // ���ֿռ��ӷ���ջ
    map<string, Stack*> spaces; // @����ջ  #����ջ  ����ջ
    list<tuple<string, Stack*>> uscps; // ����ʹ�õķ���ջ

    ElementStack stack;     // ��ǰ����ջ
    
    ASTFunctionDefine* fndef = nullptr; // ��ǰ���ڶ���ĺ���
    ASTTypeDefine*     tydef = nullptr; // ��ǰ���ڶ��������
    map<string, TypeFunction*>  lambda_funcs; // lambda ����������
    map<string, TypeFunction*>  func_param_lambdas; // ��lambda�����ĺ���

public:
    
    Stack(Stack*p=nullptr, Mod t=Mod::Anonymous);

    // ��ʼ����������ջ
    void Initialize();
    
public:

    // ��ӡ����ջ
    void print(const string &ind="");
    
    // ��������ջ
    Element* put(const string &, Element*); // ���뵱ǰջ
    Element* set(const string &, Element*, bool up = true);
    Element* find(const string &, bool up = true);
    TypeFunction* findLambda(const string &, bool up = true); // ���� lambda
    // �������ֿռ�
    Stack * use(const string &);
    // ��ѯ�����Ƿ��壬���ض���
    ASTFunctionDefine* findFunction(TypeFunction*);
    
    // ������û����庯��
    void addFunction(TypeFunction*, ASTGroup*);
    void addFunction(ASTFunctionDefine*);
    void addBuiltinFunction(const string &); // ͨ���ַ�������ڽ�����

};

#undef Namespace


}
}
