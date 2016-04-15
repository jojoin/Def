#pragma once


#include <vector>
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
    
    Stack(Stack*p=nullptr);


    // ��ʼ����������ջ
    void Initialize();
    
public:

    // ��ӡ����ջ
    void print();
    
    // ��������ջ
    Element* put(const string &, Element*); // ���뵱ǰջ
    Element* set(const string &, Element*, bool up = true);
    Element* find(const string &, bool up = true);
    // ��ѯ�����Ƿ��壬���ض���
    ASTFunctionDefine* findFunction(TypeFunction*);
    
    // ������û����庯��
    void addFunction(TypeFunction*, ASTGroup*);
    void addFunction(ASTFunctionDefine*);
    void addBuiltinFunction(const string &); // ͨ���ַ�������ڽ�����

public:
    
    Stack* parent = nullptr; // ������ջ

    ElementStack stack;     // ��ǰ����ջ

    // �Ƿ����ӷ���ջ
    bool child_scope = false;
    
    ASTFunctionDefine* fndef = nullptr; // ��ǰ���ڶ���ĺ���
    ASTTypeDefine*    tydef  = nullptr; // ��ǰ���ڶ��������


};

#undef Namespace


}
}
