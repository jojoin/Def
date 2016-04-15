#pragma once


#include <set>
#include <list>
#include <vector>
#include <string>

#include "../core/ast.h"
#include "../core/type.h"
#include "./element.h"
#include "./stack.h"
#include "./tokenizer.h"


namespace def {
namespace parse {
    
using namespace std;
using namespace def::core;


/**
 * �﷨����������
 */
class Service
{
    

public:

    Service(Tokenizer * t);
    
    Tokenizer * tkz; // �ʷ�������
    
    Stack * stack; // ��ǰ����ջ
    map<TypeStruct*, Stack*> type_member_stack; // ���Աջ
    map<TypeStruct*, ASTTypeDefine*> type_define;  // ���Աջ

    // ��ȡһ������
    Tokenizer::Word getWord();
    void prepareWord(const Tokenizer::Word &); // Ԥ��
    void prepareWord(list<Tokenizer::Word>); // ����Ԥ��

public: // �����ͺ���
    
    static bool checkType(Type*, AST*); // �����ж�

public:

    // string fixNamespace(const string &); // ׷�����ֿռ�
    // ��鲢���� include �ļ��ľ���·��
    bool checkSetInclude(const string&);

    // �ȽϺ�������ֵ����
    void verifyFunctionReturnType(Type*); 
    
    // �Ϸ��� struct array ����
    static TypeStruct* validTypeStruct(AST*);
    static TypeArray* validTypeArray(AST*);
    static Type* validType(AST*); // �Ϸ��Ķ�������
    static Type* validType(Type*); // �Ϸ��Ķ�������


public:
    
    // �Ƿ�Ϊ���캯��״̬
    bool status_construct = false;
    // ģ�����״̬
    string tpl_fun_name = ""; // ģ�庯����������
    string tpl_ty_name = ""; // ģ�����������
    
    // ������״̬
    bool is_mod_adt = false;  // �Ƿ�Ϊ������ģʽ

public:
    
    void setModADT(bool); // ���� adt ״̬
    bool checkModADT(); // ��鲢���� adt ״̬
    


    
public:
    // ����/Ԥ���ĵ���
    list<Tokenizer::Word> prepare_words; 

    set<string> includes; // �Ѿ� include �������ļ�

    // string defspace;       // ��ǰ��������ֿռ�
    // vector<string> usespaces;   // ����ʹ�õ����ֿռ�
    
};



}
}

