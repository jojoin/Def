/**
 *
 */

#include "../core/type.h"
#include "../core/error.h"
#include "./service.h"

#include "../sys/debug.h"


using namespace std;
using namespace def::core;
using namespace def::parse;
using namespace def::sys;


#define Type def::parse::Type


/**
 * ����
 */
Service::Service(Tokenizer * t)
    : tkz(t)
{
    // ��ʼ������ջ
    stack = new Stack(nullptr);
    tpf_stk = new Stack(nullptr);  // ����ģ��
    tpty_stk = new Stack(nullptr); // ��ģ��
}


/**
 * �Ϸ��� struct array ����
 */
TypeStruct* Service::validTypeStruct(AST* val)
{
    Type* tar = val->getType();
    if (auto*p=dynamic_cast<TypePointer*>(tar)) {
        tar = p->type;
    }
    return dynamic_cast<TypeStruct*>(tar);
}

TypeArray* Service::validTypeArray(AST* val)
{
    Type* tar = val->getType();
    if (auto*p=dynamic_cast<TypePointer*>(tar)) {
        tar = p->type;
    }
    return dynamic_cast<TypeArray*>(tar);
}
    
// �Ϸ��Ķ�������
Type* Service::validType(AST* val)
{
    Type* tar = validTypeStruct(val);
    if (tar) {
        return tar;
    }
    tar = validTypeArray(val);
    if (tar) {
        return tar;
    }
    return val->getType();
}
Type* Service::validType(Type* ty) {
    if (auto*p = dynamic_cast<TypePointer*>(ty)) {
        return p->type;
    }
    return ty;
}



/**
 * �ȽϺ�������ֵ����
 */
void Service::verifyFunctionReturnType(Type* ret)
{
    auto *fndef = stack->fndef;
    if ( ! fndef) {
        FATAL("Non existence function cannot return value !")
    }
    if (status_construct && ret && !dynamic_cast<TypeNil*>(ret)) { // ���캯�������з���ֵ
        FATAL("class construct function cannot have any return "+ret->str()+" value !")
    }
    if ( ret && ! fndef->ftype->ret) {
        fndef->ftype->ret = ret;
        return;
    }
    if ( ret && ! ret->is(fndef->ftype->ret)) {
        // ��������ֵ������ƥ��
        FATAL("Function '"+fndef->ftype->name+"' return value type not match !")
    }
}



/**
 * ��ѯ����
 * up: �Ƿ����ϲ���
 *
string Service::fixNamespace(const string & name)
{
    if (defspace=="") {
        return name;
    }

    return defspace + "_" + name;
}
*/




/**
 * ��鲢���� include �ļ��ľ���·��
 */
bool Service::checkSetInclude(const string& path)
{
    if (includes.find(path) == includes.end()) {
        includes.insert(path);
        return false;
    }
    // �Ѱ���
    return true;
}


    
/**
 * ��ȡһ������
 */
Tokenizer::Word Service::getWord()
{
    // ����Ԥ��
    if (!prepare_words.empty()) {
        auto rt = prepare_words.front();
        prepare_words.pop_front();
        return rt;
    }

    // ��ȡ�´�
    Tokenizer::Word word  = tkz->gain();
    // ���Դ�ӡ token list
    DEBUG_WITH("tok_list", cout << word.value << " ";)
    DEBUG_WITH("token_list", cout << (int)word.state << " " << word.value << endl;)

    return word;
}


/**
 * ����һ�����ʵ�Ԥ���б�
 */
void Service::prepareWord(const Tokenizer::Word & wd)
{
    prepare_words.push_front(wd);
}


/**
 * ����һ���б�Ԥ���б�
 */
void Service::prepareWord(list<Tokenizer::Word> wds)
{
    // ���� �� ���µ�Ԥ������뵽��ͷ
    prepare_words.splice(prepare_words.begin(), wds);
}



/**
 * �����ж�
 */
bool Service::checkType(Type* type, AST* ast)
{
    return type->is(ast->getType());
}



/**
 * ��鲢���� adt ״̬
 */
bool Service::checkModADT()
{
    bool sta = is_mod_adt;
    is_mod_adt = false;
    return sta;
}

/**
 * ���� adt ״̬
 */
void Service::setModADT(bool s=true)
{
    is_mod_adt = s;
}



/**
 * ȫ��Ψһ����
 */
string Service::uniqueName(const string& prefix)
{
    static long auto_idx = 0; // ����Ψһ��ʶ������
    auto_idx++;
    return prefix + "_" + Str::l2s(auto_idx);
}