/**
 *
 */


#include "../core/type.h"
#include "./stack.h"
#include "./service.h"
#include "../core/builtin.h"
#include "../core/error.h"
#include "../sys/debug.h"


using namespace std;
using namespace def::parse;
using namespace def::core;
using namespace def::compile;



    
/**
 * ��ȡһ������
 */
Stack::Stack(Stack* p)
    : parent(p)
{
    // ��������ջ���ʼ��
    if (!p) {
        Initialize();
    } else {
        tydef = p->tydef;
        fndef = p->fndef;
    }

}


// ��ʼ����������ջ
void Stack::Initialize()
{
    // ���ԭ�����͵�ջ��
#define AT(T) stack[#T] = new ElementType(Type::get(#T));
    DEF_AOTM_TYPE_LIST(AT)
#undef AT

    // ����ڽ�����
#define BC(N,P) addBuiltinFunction(#N "," P);
    BUILTIN_FUNCTION_LIST(BC,BC)
#undef BC

}

/**
 * ����ڽ�����
 */
void Stack::addBuiltinFunction(const string & identify)
{
    auto *fty = new TypeFunction("");
    vector<string> spx;
    Str::split(identify, ",", spx);
    int i(0);
    for (auto &it : spx) {
        if (i==0) {
            fty->name = it;
        } else if (i==1) {
            fty->set(Type::get(it));
        } else {
            fty->add(Type::get(it)); 
        }
        i++;
    }
    // �ڽ������ĺ�����Ϊ��
    // cout << fty->str() << endl;
    addFunction(fty, nullptr);
}


/**
 * ��Ӻ���
 */
void Stack::addFunction(TypeFunction* fty, ASTGroup* body)
{
    ASTFunctionDefine* fndef = new ASTFunctionDefine(fty, body);
    addFunction(fndef);
}


/**
 * ��Ӻ���
 */
void Stack::addFunction(ASTFunctionDefine* fndef)
{
    auto *fty = fndef->ftype;
    ElementGroup *addgp;
    auto fd = stack.find(fty->name);
    if (fd != stack.end()) {
        if (addgp=dynamic_cast<ElementGroup*>(fd->second)) {

        }else {
            FATAL("Duplicate definition '"+fty->name+"' by addFunction()")
        }
    } else {
        addgp = new ElementGroup();
        stack[fty->name] = addgp; // �״γ�ʼ��
    }
    // ��Ӻ���
    
    string fntylist = fty->getIdentify();

    auto *intaddfunc = new ElementFunction(fndef);
    addgp->elms.insert(make_pair(fntylist, intaddfunc));
    // ���
}



/**
 * ��ѯ����
 * up: �Ƿ����ϲ���
 */
ASTFunctionDefine* Stack::findFunction(TypeFunction* ftype)
{
    string fname = ftype->name;
    // �жϺ����Ƿ��Ѿ�����
    if (auto *fngr = dynamic_cast<ElementGroup*>(find(fname))) {
        string fidn = ftype->getIdentify();
        auto fd = fngr->elms.find(fidn);
        if (fd != fngr->elms.end()) {
            if (auto *declare = dynamic_cast<ElementFunction*>(fd->second)) {
                // �����Ѿ�����
                return declare->fndef;
            }
        }
    }

    // ���Ҹ���ջ
    if(parent){
        return parent->findFunction(ftype);
    }

    // δ����
    return nullptr;

}





/**
 * ��ӡ����ջ
 */
void Stack::print()
{
    for (auto &li : stack) {
        if (!li.second) continue;
        cout << li.first << ": " << li.second->str() << endl;
    }
}

/**
 * ��ѯ����
 * up: �Ƿ����ϲ���
 */
Element* Stack::find(const string & name, bool up)
{
    ElementStack::iterator it;
    it = stack.find(name);
    if(it!=stack.end()){
        return it->second; // �ҵ�
    }
    // �������ֿռ��ٲ���
    it = stack.find(name);
    if(it!=stack.end()){
        return it->second; // �ҵ�
    }
    // ��ѯ��ջ
	if(up&&parent){
        auto res = parent->find(name);
        // ��¼���������������
        if (auto *ev = dynamic_cast<ElementVariable*>(res)) {
            if (fndef && !child_scope) {
                // cout << fndef->ftype->name << "  capture the variable: " << name << endl;
                fndef->cptvar[name] = ev->type;
            }
        }
        return res;
	}
    return nullptr;
}


/**
 * ��ǰ���������
 * return: ������ֵ
 */
Element* Stack::put(const string & name, Element* elem)
{
    ElementStack::iterator it;
    it = stack.find(name);
    if(it==stack.end()){
        stack.insert(ElementStack::value_type(name, elem));
        return nullptr; // ����������
    }
    Element* old = it->second;
    stack[name] = elem;
    return old; // ���ؾ�����ֵ
}


/**
 * ��������
 * ���������Ѿ�����
 * up: �Ƿ����ϲ�������
 * return: ������=�ɹ�  NULL=ʧ��
 */
Element* Stack::set(const string & name, Element* elem, bool up)
{
    ElementStack::iterator it;
    it = stack.find(name);
    if(it!=stack.end()){ // �ҵ�������
        Element* old = it->second;
        stack[name] = elem;
        return old;
    }
    if(up&&parent){
        return parent->set(name, elem, true); // �鸸��
    }
    return nullptr; // û�ҵ�
}

