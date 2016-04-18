/**
 * �﷨�����ռ�
 */


#include <stack>

#include "build.h"
#include "../core/error.h"

#include "../util/path.h"
#include "../util/fs.h"
#include "../sys/debug.h"

#include "../core/splitfix.h"
#include "../core/ast.h"
#include "../core/builtin.h"

#include "../parse/filter.h"


using namespace std;
using namespace def::core;
using namespace def::util;
using namespace def::sys;
using namespace def::compile;
using namespace def::parse;


#define Word Tokenizer::Word 
#define State Tokenizer::State

#define ISWS(TS) word.state==State::TS
#define NOTWS(TS) !(ISWS(TS))
#define ISSIGN(S) ISWS(Sign)&&word.value==S
#define NOTSIGN(S) !(ISSIGN(S))
#define ISCHA(S) ISWS(Character)&&word.value==S

// �������
#define CHECKLPAREN(T) word = getWord(); \
    if (NOTSIGN("(")) { FATAL(T) }
#define CHECKRPAREN(T) word = getWord(); \
    if (NOTSIGN(")")) { FATAL(T) }

    
/**
 * ����
 */
Build::Build(Tokenizer * t)
    : Service(t)
{
}

    
/**
 * �����Ԥ���������ڵ�
 */
void Build::prepareBuild(AST* p)
{
    if (p) {
        prepare_builds.push_back(p);
    }
}
void Build::prepareBuild(const list<AST*> & asts)
{
    for (auto &li : asts) {
        prepareBuild(li);
    }
}


/********************************************************/


/**
 * ��������
 */
ASTGroup* Build::createAST()
{
    return buildGroup();
}

/**
 * ���� Group
 */
ASTGroup* Build::buildGroup()
{
    // �½�������
    ASTGroup* block = new ASTGroup();
    while (1) {
        AST* li = build();
        if (nullptr==li) {
            break;
        }
        block->add(li);
    }
    return block;
}

/**
 * �������ʽ
 * spread �Ƿ���Ҫ��� let ���Ű�
 */
AST* Build::build(bool spread)
{
    
    // ���Դ�ӡ
    DEBUG_WITH("prepare_words", \
        if(prepare_words.size()){ \
        cout << "prepare��"; \
        for (auto &p : prepare_words) { \
            cout << " " << p.value; \
        } \
        cout << "��" << endl; \
        } \
        )


    if (!prepare_builds.empty()) {
        auto rt = prepare_builds.front();
        prepare_builds.pop_front();
        return rt;
    }

    // ������չ��
    if (spread) {
        if (AST* res = buildOperatorBind()) {
            return res;
        }
    }

    Error::snapshot();

    Word word = getWord();
    
    if ( ISWS(End) ) {
        return nullptr; // �ʷ�ɨ�����
    }


    // ������
    if (ISSIGN(";")) {
        return build();
        
    // group
    } else if(ISSIGN("(")){
        //cout << "create new scope !" << endl;
        // �½�����ջ
        //auto * old_stack = stack;
        //stack = new Stack(old_stack);
        ASTGroup* grp = buildGroup();
        // ��λ����ջ
        //delete stack;
        //stack = old_stack;
        // ������֤
        word = getWord();
        if (NOTSIGN(")")) {
            FATAL("Group build not right end !")
        }
        if (grp->childs.size()==1) {
            AST* res = grp->childs[0];
            delete grp;
            return res;
        }
        return grp;

    // Number
    } else if (ISWS(Number)) {
        Type *nty = Type::get("Int");
        if (Tokenizer::isfloat(word.value)) {
            nty = Type::get("Float");
        }
        return new ASTConstant(nty, word.value);
        
    // Char
    } else if (ISWS(Char)) {
        return new ASTConstant(Type::get("Char"), word.value);
     
    // String
    } else if (ISWS(String)) {
        return new ASTConstant(Type::get("String"), word.value);
        
    // Character
    } else if ( ISWS(Character) ) {
        
        string chara = word.value;

        // ��ѯ��ʶ���Ƿ���
        Element* res = stack->find(chara, true);
        
        // �������� ��
        if (auto val = buildVaribale(res, chara)) {
            return val;
        }

        // ��� ��
        if (auto gr = dynamic_cast<ElementLet*>(res)) {
            return buildMacro(gr, chara);
        }
        
        // ���͹��� ��
        if (auto gr = dynamic_cast<ElementType*>(res)) {
            return buildConstruct((TypeStruct*)gr->type);
        }

        // �������� ��
        if (auto gr = dynamic_cast<ElementGroup*>(res)) {
            auto fncall = _functionCall(chara ,stack);
            if (fncall) {
                return fncall;
            }
        }
        
        // ��ģ����ã�
        if(auto gr = dynamic_cast<ElementTemplateType*>(res)){
            return buildTemplateType(chara, gr);
        }

        // ����ģ����ã�
        res = stack->find(DEF_PREFIX_TPF+chara, true);
        if (res) {
            return buildTemplateFuntion(chara, (ElementTemplateFuntion*)res);
        }

        // ���Ժ��Ķ��壿
        AST *core = buildCoreDefine(chara);
        if (core) {
            return core;
        }

        // Ϊ Nil �������泣��
        if (chara=="nil") {
            return new ASTConstant(Type::get("Nil"), word.value);
        }

        // Ϊ Bool �������泣��
        if (chara=="true"||chara=="false") {
            return new ASTConstant(Type::get("Bool"), word.value);
        }

        // ����
        Error::exit("Undefined identifier: " + chara);
    }
    

    Error::backspace(); // ��λ

    // ���浥��  �����ϲ㴦��
    // cacheWord(word);
    prepareWord(word);

    return nullptr; // �ʷ�ɨ�����


}
    

/********************************************************/


/**
 * ���������Ա����
 */
AST* Build::buildVaribale(Element* elm, const string & name)
{
    if (auto gr = dynamic_cast<ElementVariable*>(elm)) {
        auto *val = new ASTVariable( name, gr->type );
        val->origin = gr->origin;
        // ȫ��Ψһ����
        val->unique_name = gr->unique_name;
        return val;
    }

    // �Ƿ�Ϊ���Ա����
    if(stack->tydef && stack->fndef){
        AST* instance = new ASTVariable( // �����
            DEF_MEMFUNC_ISTC_PARAM_NAME,
            stack->tydef->type
        );
        // �����౾��
        if (name==DEF_MEMFUNC_ISTC_PARAM_NAME) {
            stack->fndef->is_static_member = false; // �г�Ա����
            return instance;
        }
        int i = 0;
        for (auto &p : stack->tydef->type->tabs) {
            if (p==name) { // �ҵ�
                // �������Ա����
                AST* elmget = new ASTMemberVisit(instance, i);
                stack->fndef->is_static_member = false; // �г�Ա����
                // �Ƿ�Ϊȡ����ֵ
                if (auto * qty = dynamic_cast<TypeRefer*>(stack->tydef->type->types[i])) {
                    if (!qty->type) { // ��ʼ��֮ǰ����ʹ��
                        FATAL("Can't use quote value before initialize !")
                    }
                    return new ASTLoad(elmget, qty); // ��������
                }
                return elmget;
            }
            i++;
        }
    }


    return nullptr;
}

/**
 * ���Ķ��崦��
 */
AST* Build::buildCoreDefine(const string & name)
{
    /** def ���Ķ����б� **/
#define T(N) if(#N==name) return build_##N();
    BUILTIN_DEFINES_LIST(T)
#undef T

    // δƥ��
    return nullptr;

}

/**
 * ����ģ�庯��
 */
AST* Build::buildTemplateFuntion(const string & name, ElementTemplateFuntion* tpf)
{
    // ��ѯ tpf_stk ����ģ��ջ����
    string tpfid("");
    // �º�������
    TypeFunction* functy = new TypeFunction(name);
    // �����º���
    auto *fndef = new ASTFunctionDefine(functy);
    // ����ģ�庯������
    auto *fncall = new ASTFunctionCall(fndef);
    // �����·���ջ
    Stack* old_stack = stack;
    auto * new_stack = new Stack(stack, Stack::Mod::Function); // ��������ջ
    // ʵ����ջ
    for (auto &pn : tpf->tpfdef->params) {
        AST* p = build();
        fncall->addparam(p);
        Type *ty = p->getType();
        functy->add(ty, pn); // ��������
        new_stack->put(pn, new ElementVariable(ty)); // ��ʵ��
        // ����key
        tpfid += "," + Str::l2s((int)ty);
    }
    tpfid[0] = '(';
    tpfid = Str::l2s((int)tpf->tpfdef) + tpfid + ")";


    // ��ѯ����
    if(auto elmfdef=dynamic_cast<ElementFunction*>(tpf_stk->find(tpfid))){
        // ������������
        // cout << "return buildTemplateFuntion cache: " << tpfid << endl;
        delete functy;
        delete fndef;
        fncall->fndef = elmfdef->fndef;
        // ���浽��ǰջ
        stack->addFunction(elmfdef->fndef);
        return fncall;
    }
    
    // �½���������
    fndef->wrap = stack->fndef; // wrap
    new_stack->fndef = fndef; // ��ǰ����ĺ���
    // �滻��ջ֡
    stack = new_stack;
    // Ԥ�����������
    prepareWord(tpf->tpfdef->bodywords);
    // ����������
    ASTGroup *body = createAST();
    auto word = getWord(); 
    if(NOTSIGN(")")){
        FATAL("Error format function body !)")
    } 
    // ���� Body
    fndef->body = body;
    // �Զ���ӷ���ֵ��䣬 ��鷵��ֵ����һ����
    Type* tyret = _autoAddFuncRet(fndef);
    // ���ÿ��ܱ�ǵķ���ֵ
    functy->ret = tyret;
    fndef->ftype = functy;
    // ��λ��ջ֡
    stack = old_stack;
    // ����º���
    stack->addFunction(fndef);
    // ��ӵ� tpf_stk ����ģ�建��ջ
    // cout << "put cache: " << tpfid << endl;
    tpf_stk->put(tpfid, new ElementFunction(fndef));
    // ���غ�������
    return fncall;
}

/**
 * ����ģ����
 */
AST* Build::buildTemplateType(const string & name, ElementTemplateType* tpty)
{
    // ��ȡ����
    TypeStruct* claty = _templateType(tpty->tptydef);

    // ���ͳ�ʼ��
    return buildConstruct(claty);
}

/**
 * �������캯������
 */
AST* Build::buildConstruct(TypeStruct* tyclass, AST* vptr)
{
    string name = tyclass->name;

    // ���캯������
    string fname = DEF_PREFIX_CSTC + name;

    // auto *tyclass = (TypeStruct*)ety->type;
    // �����Ƿ���ڹ��캯��
    auto fd = stack->find(fname);

    // �޹��캯��ʱ
    if (!fd) {

        auto *cst = new ASTTypeConstruct(tyclass);
        cst->instance = vptr; // ���ڴ�
        int plen = tyclass->len();
        while (plen--) { // ������͹������
            cst->add(build());
        }
        return cst;
    }

    // ʹ���ѷ�����ڴ� �� ���ÿչ��캯��
    auto *val = vptr ? vptr : new ASTTypeConstruct(tyclass, true); // �չ���

    // �����ڲ�ջ
    auto target_stack = type_member_stack[tyclass];

    // ��������
    ASTFunctionCall* fncall =
        _functionCall(fname ,target_stack, false);

    // δ�ҵ����ʵĹ��캯��
    if ( ! fncall) {
        FATAL("can't match class '"+tyclass->name
            +"' construct function '"
            +fname+"' !")
    }


    /*
    // ��ѯĿ�����Ա����
    // auto word = getWord();
    auto target_stack = type_member_stack[tyclass];
    // ��Ա�������ã������ϲ���
    auto filter = filterFunction(target_stack, funname, false);
    if (!filter.size()) {
        FATAL("can't find member function '"
            +funname+"' in class '"+tyclass->name+"'")
    }


    // string name = word.value;
    auto *fncall = new ASTFunctionCall(nullptr);
    // auto elms = grp->elms;
        
    ASTFunctionDefine* fndef(nullptr);

    // ��ʱ������ѯ����������
    auto *tmpfty = new TypeFunction(funname);


    // ������Ա��������

        // ����̰��ƥ��ģʽ
    while (true) {
        // ƥ�亯������һ���޲Σ�
        int match = filter.match(tmpfty);
        if (match==1 && filter.unique) {
            fndef = filter.unique; // �ҵ�Ψһƥ��
            string idname = fndef->ftype->getIdentify();
            break;
        }
        if (match==0) {
            //prepareBuild(cachebuilds);
            FATAL("No macth function '"+tyclass->name+"."+tmpfty->getIdentify()+"' !");
            // throw ""; // ��ƥ��
        }
        // �жϺ����Ƿ���ý���
        auto word = getWord();
        prepareWord(word); // �ϲ㴦������
        if (ISWS(End) || ISSIGN(")")) { // ��ǰ�ֶ����ý���
            if (filter.unique) {
                fndef = filter.unique; // �ҵ�Ψһƥ��
                break;
            }
        }
        // ��Ӳ���
        AST *exp = build();
        if (!exp) {
            if (filter.unique) {
                fndef = filter.unique; // �޸���������ҵ�ƥ��
                break;
            }
            FATAL("No macth function '"+tyclass->name+"."+tmpfty->getIdentify()+"' !");
        }
        //cachebuilds.push_back(exp);
        fncall->addparam(exp); // ��ʵ��
        auto *pty = exp->getType();
        // ���غ�����
        tmpfty->add("", pty);
    }

    fncall->fndef = fndef; // elmfn->fndef;
        

    delete tmpfty;



    // ��̬��Ա������֤
    //if (is_static && ! fndef->is_static_member) {
    //    FATAL("'"+fndef->ftype->name+"' is not a static member function !")
    //}
    */
    
    auto * mfc = new ASTMemberFunctionCall(val, fncall);

    return mfc;
}

/**
 * �����
 */
AST* Build::buildMacro(ElementLet* let, const string & name)
{
    // ��������
    map<string, list<Word>> pmstk;
    for (auto pm : let->params) {
        list<Word> pmws;
        auto word = getWord();
        if(ISSIGN("(")){ // �����������
            cacheWordSegment(pmws);
        } else {
            pmws.push_back(word);
        }
        pmstk[pm] = pmws;
    }

    // չ������
    list<Word> bodys;
    for (auto &word : let->bodywords) {
        if(NOTWS(Character)){
            bodys.push_back(word);
            continue;
        }
        auto fd = pmstk.find(word.value);
        if (fd != pmstk.end()) {
            for (auto &w : fd->second) {
                bodys.push_back(w);
            }
        } else {
            bodys.push_back(word);
        }
    }

    // Ԥ��
    prepareWord(bodys);
    // ���Դ�ӡ
    DEBUG_WITH("letmacro_words", \
        if(bodys.size()){ \
        cout << "letmacro��"; \
        for (auto &p : bodys) { \
            cout << " " << p.value; \
        } \
        cout << "��" << endl; \
        } \
        )

    // ���¿�ʼ����
    return build();
}

/**
 * ������������
 */
AST* Build::buildChildScope(const string & name, const string & tip)
{
    // cout << "create new scope !" << endl;
    auto* scope = new ASTChildScope(name);
    auto * old_stack = stack;
    // ���ֿռ�
    string scopename = name;
    // �ж��ض���
    auto it = old_stack->spaces.find(scopename);
    if(it!=old_stack->spaces.end()){
        // FATAL("Namespaces '"+scopename+"' cannot redefinition ��")
        stack = it->second; // ����ջ�ϲ�
    } else {
        // �½�����ջ
        stack = new Stack(old_stack, Stack::Mod::Namespace);
    }
    if(name==""){
        static int anyidx = 0; // �������
        anyidx++;
        scopename = "#" + tip + Str::l2s(anyidx);
        stack->mod = Stack::Mod::Anonymous; // ��������ջ
    }
    // �����ջ
    old_stack->spaces[scopename] = stack;

    // ��ʼ��֤
    auto word = getWord();
    if (ISSIGN("(")) {
        while (1) {
            AST* li = build();
            if (nullptr==li) {
                break;
            }
            scope->childs.push_back(li);
        }
        word = getWord();
        if (NOTSIGN(")")) {
            FATAL("Child scope not right end !")
        }
    } else {
        prepareWord(word);
        AST* li = build();
        scope->childs.push_back(li);
    }
    // ��λ����ջ
    stack = old_stack;
    // ���������������
    return scope;
}


/********************************************************/


/**
 * namespace �������ֿռ�
 *
AST* Build::build_namespace()
{

    
    Word word = getWord();

    if (NOTWS(Character)) {
        FATAL("namespace define need a legal name to belong !")
    }

    // ��ʷ����
    int oldlen = defspace.size();

    // ��������ռ�
    defspace += "";// (oldlen == 0 ? "" : "_") + word.value;

    CHECKLPAREN("namespace define need a sign ( to belong !")

    // ѭ���������
    AST *block = createAST();

    CHECKRPAREN("namespace define need a sign ) to end !")

    // ���ֿռ临λ
    defspace = defspace.substr(0, oldlen);

    return block;
}
*/


/**
 * include ������չ���ļ�
 */
AST* Build::build_include()
{
    Error::snapshot();
    AST* path = build(false);
    auto *p = dynamic_cast<ASTConstant*>(path);
    if (!p || !p->type->is(Type::get("String"))) {
        // ������Ч��·��
        Error::exit("Behind 'include' is not a valid file path ! "\
            "(need a type<String> constant value)");
    }

    // �ӵ�ǰ�������ļ���λ����һ���ļ�
    string absfile = Path::join(tkz->file, p->value);
    // cout << "absfile:" << absfile << endl;
    if (!Fs::exist(absfile)) {
        Error::exit("Cannot find the file '"+p->value+"' at current and library path ! ");
    }
    
    Error::backspace(); // ����

    // include Ψһ��
    if (checkSetInclude(absfile)) {
        // ���Դ�ӡ
        DEBUG_COUT("repeat_include", "[repeat include] "+absfile)

        delete path;
        return new ASTGroup();
    }


    // ���𻺴�ĵ���
    list<Word> cache = prepare_words;
    prepare_words.clear();

    // �½����滻�ʷ�������
    auto *old_tkz = tkz;
    Tokenizer new_tkz(absfile, false);
    tkz = & new_tkz;

    // ִ���﷨����
    AST* tree = createAST();

    // ��λ�ʷ�������
    tkz = old_tkz;

    // ��λ����ĵ���
    prepare_words = cache;

    // ��λ���󱨸�
    Error::update(old_tkz);

    delete p;
    return tree;
}

/**
 * ��ʼ������
 */
AST* Build::build_var()
{    
    Word word = expectIdName("var define need a legal name to belong !");

    string name = word.value;

    // �����Ƿ����
    auto fd = stack->find(name, false);
    if (fd) {
        FATAL("Cannot repeat define var '"+name+"' !")
    }
    
    AST* value = build();

    auto vardef = new ASTVariableDefine( name, value );

    // ���ֵ�Ǳ���
    auto *var = new ElementVariable(value->getType());
    if (auto*astv=dynamic_cast<ASTVariable*>(value)) {
        var->origin = astv->origin;
    } else {
        var->origin = value;
    }
    //ȫ��Ψһ����
    ASSERT(vardef->unique_name!="", "")
    var->unique_name = vardef->unique_name;
    // ��ӱ�����ջ
    stack->put(name, var);

    return vardef;
}

/**
 * set ������ֵ
 */
AST* Build::build_set()
{
    Word word = getWord();

    if (NOTWS(Character)) {
        FATAL("var assignment need a legal name to belong !")
    }

    string name = word.value;

    // ���ұ����Ƿ����
    auto fd = stack->find(name);
    ElementVariable * ev = dynamic_cast<ElementVariable*>(fd);
    if ( !fd || !ev ) {
        // ���������ڣ������Ƿ�Ϊ��Ա����
        if (stack->tydef) {
            AST* instance = new ASTVariable( // �����
                DEF_MEMFUNC_ISTC_PARAM_NAME,
                stack->tydef->type
            );
            // ��ֵ�౾��
            if (name==DEF_MEMFUNC_ISTC_PARAM_NAME) {
                stack->fndef->is_static_member = false; // �г�Ա����
                return instance;
            }
            Type* elmty = validType(stack->tydef->type->elmget(name));
            if (elmty) { // �ҵ���Ա
                AST* value = build();
                // ��������
                auto * qty = dynamic_cast<TypeRefer*>(elmty);
                // ���ͼ��
                auto* chty = elmty;
                if (qty) {
                    chty = qty->type;
                }
                if ( ! validType(value->getType())->is(chty)) {
                    FATAL("member assign type not match !")
                }
                stack->fndef->is_static_member = false; // �г�Ա����
                // �������Ա��ֵ
                Type *elmty = stack->tydef->type->elmget(name);
                // �Ƿ�Ϊ������ֵ
                if (qty) {
                    if (!qty->type) { // ��ʼ��֮ǰ����ʹ��
                        FATAL("Can't use quote value before initialize !")
                    }
                    value = new ASTQuote(value, qty); // ��������
                }
                // ���ط���
                return new ASTMemberAssign(
                    instance,
                    stack->tydef->type->elmpos(name),
                    value
                );
            }
        }

        FATAL("var '" + name + "' does not exist can't assignment  !")
    }

    // ����
    Type* ty = ev->type;

    // ֵ
    AST* value = build();
    Type* vty = value->getType();

    // ���ͼ��
    if (!vty->is(ty)) {
        FATAL("can't assignment <"
            +vty->getIdentify()+"> to var "
            +name+"<"+ty->getIdentify()+">  !")
    }

    // ��ӱ�����ջ
    // stack->set(name, new ElementVariable(vty));

    auto * ret = new ASTVariableAssign( name, value );
    // ȫ��Ψһ����
    ret->unique_name = ev->unique_name;
    return ret;

}

/**
 * type ��������
 */
AST* Build::build_type()
{
    if (stack->tydef) { // Ƕ�׶���
        FATAL("can't define type in type define !")
    }
    /*
    Word word = getWord();
    if (NOTWS(Character)) {
        FATAL("type define need a legal name to belong !")
    }
    string typeName = fixNamespace( word.value );
    */

    Word word = expectIdName("type define need a legal name to belong !" );
    string typeName = word.value;

    // ��ѯ�����Ƿ���
    if (auto *fd = dynamic_cast<ElementType*>(stack->find(typeName,false))) {
        FATAL("can't repeat type '"+typeName+"' !")
    }

    // �������
    ;
    CHECKLPAREN("namespace define need a sign ( to belong !")

    // �½�����
    TypeStruct* tyclass = new TypeStruct(typeName);
    if (stack->fndef) {
        tyclass->increment(); // �����������Ƽ�����������
    }

    // ����AST
    ASTTypeDefine* tydef = new ASTTypeDefine();
    tydef->type = tyclass;

    auto *elmty = new ElementType(tyclass);

    // ��ջ
    Stack *old_stk = stack;
    Stack *new_stk = new Stack(stack, Stack::Mod::Class);
    // ��ӵ���ǰջ֡ ֧�ֺ�������
    new_stk->put(typeName, elmty);
    new_stk->tydef = tydef;
    new_stk->fndef = nullptr; // ���Ա�����������κΰ�������
    stack = new_stk;
    // �����ջ
    old_stk->spaces["%" + typeName] = new_stk;


#define TCADD(N) \
    if(it){ \
        tyclass->add(N, it); \
    } else { \
        FATAL("Type declare format error: not find Type <"<<word.value<<"> !") \
    }

    while (1) { // ����Ԫ�ض���
        // ��Ա����
        Type *it = expectTypeState();
        if (tyclass->is(it)) { // ���Ա���ܰ����Լ���������
            FATAL("Type declare can't contains it self unless referenced !")
        }
        if(!it) {
            word = getWord();
            if (ISSIGN(")")) {
                break; // ������������
            }
            if("fun"==word.value){
                build_fun(); continue;
            } else if ("dcl" == word.value) {
                build_dcl(); continue;
            }
            FATAL("Type declare format error: not find Type <" << word.value << "> !")
        }
        // ������Ա
        while (true) {
            if (Type *ty = expectTypeState()) {
                tyclass->add(it); // ����
                it = ty;
            } else break;
        }
        // ��Ա����
        word = getWord();
        // ��ӳ�Ա
        tyclass->add(it, word.value);
    }

#undef TCADD

    // ��鿽����Ա�����б�
    type_member_stack[tyclass] = new_stk;
    type_define[tyclass] = tydef;


    // ��ӵ�����ջ
    stack = old_stk;
    stack->put(typeName, elmty);

    // ����
    return tydef;
}

/**
 * tydef ����������
 */
AST* Build::build_tydef()
{
    // ��������
    Word word = expectIdName("type rename need a legal name to belong !" );
    string typeName = word.value;

    // ��������
    Type *ty = expectTypeState();
    if(!ty){
        FATAL("type rename need a legal type to belong !")
    }

    // ��ӽ�����ջ
    auto *elmty = new ElementType(ty);
    stack->put(typeName, elmty);

    // ��������������
    return new ASTTypeRename(ty, typeName);
}

/**
 * dcl ��������
 */
AST* Build::build_dcl()
{
    Word word = expectIdName("function declare need a type name to belong !");

    // ��������ֵ����
    Type *rty;
    Element* elmret = stack->find(word.value);
    if (auto *ety = dynamic_cast<ElementType*>(elmret)) {
        rty = ety->type;
    } else {
        FATAL("function declare need a type name to belong !")
    }

    // ��������
    word = getWord();
    if (NOTWS(Character)) {
        FATAL("function declare need a legal name !")
    }

    // �½���������
    string funcname = word.value;
    auto *functy = new TypeFunction(funcname, rty);

    // ����
    CHECKLPAREN("function declare need a sign ( to belong !")

    // ������������
    while (true) {
        word = getWord();
        if (ISSIGN(")")) {
            break; // ���������б����
        }
        Type *pty;  // ��������
        if (auto *ety = dynamic_cast<ElementType*>(stack->find(word.value))) {
            functy->add(ety->type); // ����
        } else {
            FATAL("function declare parameter format is not valid !")
        }
    }
    
    // �����Ƿ��Ѿ�����
    ASTFunctionDefine* fndef = stack->findFunction(functy);

    // �жϺ����Ƿ��Ѿ�����
    if (!fndef) {
        fndef = new ASTFunctionDefine(functy, nullptr);
        // ����º���
        stack->addFunction(fndef);
    }

    // ���غ�������
    return new ASTFuntionDeclare(fndef->ftype);
}

/**
 * fun ���庯��
 */
AST* Build::build_fun()
{

    Word word = expectIdName("function define need a type name to belong !");

    // ��������ֵ����
    Type *retty(nullptr); // nullptr ʱ��Ҫ�ƶ�����
    
    // ��������
    string funcname = word.value;

    // �Ƿ�Ϊ���͹��캯��
    if(stack->tydef && (
        funcname == tpl_ty_name ||
        funcname == stack->tydef->type->name))
    {
        auto word = getWord();
        if(ISSIGN("(")){
            // ������������ͬ�����޷���ֵ��Ϊ���캯��
            status_construct = true;
            funcname = stack->tydef->type->name;
        } else {
            prepareWord(word);
        }

    }
    
    // ��ͨ��������
    if (!status_construct) {
        Element* elmret = stack->find(word.value);
        if (auto *ety = dynamic_cast<ElementType*>(elmret)) {
            retty = ety->type;
        }
        else {
            prepareWord(word);
        }

        // ��������
        word = getWord();
        if (NOTWS(Character)) {
            FATAL("function define need a legal name !")
        }
        funcname = word.value; // ������

        // ������֤
        CHECKLPAREN("function define need a sign ( to belong !")
    }

    // �½���������
    // string funcname = word.value;
    // �����������������
    bool is_delete = stack->tydef && funcname == "delete";
    if(is_delete){
        funcname = "~delete";
    }

    string cstc_funcname = funcname;
    if (status_construct) {
        cstc_funcname = DEF_PREFIX_CSTC + funcname;
    }
    TypeFunction *functy = new TypeFunction(cstc_funcname);
    TypeFunction *constructfuncty = new TypeFunction(cstc_funcname);


    // ������������
    while (true) {
        Type *pty = expectTypeState(); // ��������
        if (!pty) {
            word = getWord();
            if (ISSIGN(")")) {
                break; // ���������б����
            }
            FATAL("Parameter format is not valid !")
        }
        string pnm; // ��������
        word = getWord();
        if (NOTWS(Character)) {
            FATAL("function parameter need a legal name !")
        }
        pnm = word.value;
        if (stack->tydef && pnm == DEF_MEMFUNC_ISTC_PARAM_NAME) {
            // ���Ա�������������Ʋ����� this 
            FATAL("can't give a parameter name '" + pnm + "' in type member function !")
        }
        functy->add(pty, pnm);
        // ���캯����ջ
        if (status_construct) {
            constructfuncty->add(pty, pnm);
        }
    }

    // �������� �����в���
    if(is_delete && functy->types.size()){
        FATAL("Destructors cannot have parameters !")
    }

    // �����Ƿ��Ѿ�����
    ASTFunctionDefine* aldef = stack->findFunction(functy);
    ASTFunctionDefine *fndef;

    // �жϺ����Ƿ��Ѿ�����
    if (aldef){
        if (aldef->body) {
            // �������������� body �Ѷ���
            FATAL("function define repeat: " + functy->str());
        }
        fndef = aldef;
    } else {
        // �½���������
        fndef = new ASTFunctionDefine(functy);
    }

    // �����·���ջ
    Stack* old_stack = stack;
    auto *new_stack = new Stack(stack, Stack::Mod::Function); // ��������ջ
    new_stack->fndef = fndef; // ��ǰ����ĺ���
    // ��ǰ ��Ӻ��� ֧�ֵݹ�
    stack->addFunction(fndef);

    // �����ջ
    old_stack->spaces["@"+funcname] = new_stack;

    // �������廷��
    fndef->wrap = stack->fndef; // wrap
    fndef->belong = stack->tydef; // belong

    // ��Ӻ���ʵ�ε�����ջ
    int i(0);
    for (auto &pty : functy->types) {
        string pn(functy->tabs[i]);
        //string unpn = ASTVariableDefine::getUniqueName();
        auto ev = new ElementVariable(pty);
        // ����ȫ��Ψһ���ƣ������ڲ����������򲶻�
        //ev->unique_name = unpn;
        new_stack->put(pn, ev); // ��ʵ��
        //new_stack->put(unpn, ev); // ��ȫ��Ψһʵ��
        i++;
    }
    // �滻��ջ֡
    stack = new_stack;

    // �½�������
    ASTGroup *body;
    word = getWord();
    // �����
    if (ISSIGN("(")) {
        body = buildGroup();
        word = getWord();
        if (NOTSIGN(")")) {
            FATAL("Error format function body !")
        }
    // �����
    } else {
        body = new ASTGroup();
        body->add(build());
    }
    
    // ���� Body
    fndef->body = body;
    // �Ƿ�Ϊ���캯��
    fndef->is_construct = status_construct;

    // ����ֵ��֤���ƶϣ��Զ���ӷ���ֵ���
    retty = _autoAddFuncRet(fndef);

    // ���ÿ��ܱ�ǵķ���ֵ
    /*
    if (auto*astv=dynamic_cast<ASTVariable*>(cur_fun_ret)) {
        cur_fun_ret = astv->origin;
    }
    // ���ض��ڴ洴���Ķ���
    if (dynamic_cast<ASTNew*>(cur_fun_ret)) {
        retty = new TypePointer(retty);
    }
    */
    functy->ret = retty;
    fndef->ftype = functy;
    
    // ��λ��ջ֡
    stack = old_stack;

    // ���캯����ջ
    if (status_construct) {
        auto * main_stack = old_stack->parent;
        constructfuncty->ret = stack->tydef->type;
        auto *constrct = new ASTFunctionDefine(
            constructfuncty, body);
        constrct->is_construct = true;
        main_stack->addFunction(constrct);
        // 
        fndef->is_construct = true;
        fndef->is_static_member = false;
    } else {
        delete constructfuncty;
    }

    // �����Ѿ��������ˣ���� body
    if (aldef) {
        // �滻 ftype�������������
        // delete aldef->ftype;
        // aldef->ftype = functy;

    // ��������º���
    } else {
        stack->addFunction(fndef);
    }

    // ��ӵ����Ա����
    if(stack->tydef){
        stack->tydef->members.push_back(fndef);
    }
    
    // ��λ
    status_construct = false;

    // ����ֵ
    return fndef;
}

/**
 * ret ����ֵ����
 */
AST* Build::build_ret()
{
    AST *ret = build();
    Type* rty = ret->getType();
    // ��֤����ֵ
    verifyFunctionReturnType( rty );
    // cur_fun_ret = ret;
    return new ASTRet(ret);
}

/**
 * tpf ģ�庯���ƶ�
 */
AST* Build::build_tpf()
{
    Word word = expectIdName("template function define need a legal name to belong !");
    
    string tpfName = word.value; // fixNamespace(word.value);
    if (stack->find(DEF_PREFIX_TPF + tpfName,false)) {
        FATAL("template function duplicate definition '"+tpfName+"' !");
    }
    
    // �������
    CHECKLPAREN("template function define need a sign ( to belong !")

    // �½�����
    auto *tpfdef = new ASTTemplateFuntionDefine();
    tpfdef->name = tpfName;

    // �����β��б�
    while (1) {
        word = getWord();
        if (ISSIGN(")")) { // �β��б����
            break;
        }
        if (NOTWS(Character)) {
            FATAL("template function define parameter name is not valid !")
        }
        tpfdef->params.push_back( word.value );
    }

    word = getWord();
    if (NOTSIGN("(")) {
        FATAL("template function define body need a sign ( to belong !")
    }
    
    // ����������
    cacheWordSegment(tpfdef->bodywords, false); // �������Ŷ�����
    
    // ��ӵ�����ջ
    stack->put(DEF_PREFIX_TPF + tpfName, new ElementTemplateFuntion(tpfdef));

    return tpfdef;
}

/**
 * tpty ��ģ��
 */
AST* Build::build_tpty()
{
    // ��ģ��
    auto *tptydef = new ASTTemplateTypeDefine();
    
    // ��ģ������
    Word word = expectIdName("template type define need a legal name to belong !");
    tptydef->name = word.value; // fixNamespace(word.value);
    
    // �������
    CHECKLPAREN("template type define need a sign ( to belong !")

    // ����ģ�����
    while(true){
        word = getWord();
        if(ISSIGN(")")){
            break;
        }
        tptydef->params.push_back(word.value);
    }
    
    // �������
    CHECKLPAREN("template type define need a sign ( to belong !")
    
    // ����ģ��ʵ��
    cacheWordSegment(tptydef->bodywords, false);

    // ��ӵ�����ջ
    stack->put(tptydef->name, new ElementTemplateType(tptydef));

    // ������ģ��
    return tptydef;
}

/**
 * if ������֧
 */
AST* Build::build_if()
{

#define DO_FATAL FATAL("No match bool function Type<"+idn+"> when be used as a if condition")
    
    AST* cond = build();
    Type* cty = cond->getType();
    Type* boolty = Type::get("Bool");

    string idn = cty->getIdentify();

    bool isboolty = cty->is(boolty);
    if ( ! isboolty) { // �� Bool ���ͣ����� bool ����
        auto *bools = dynamic_cast<ElementGroup*>(stack->find("bool"));
        if (!bools) { 
            DO_FATAL
        }
        auto fd = bools->elms.find("bool" DEF_SPLITFIX_FUNCARGV + idn);
        if (fd == bools->elms.end()) {
            DO_FATAL
        }
        auto *ef = dynamic_cast<ElementFunction*>(fd->second);
        if (!ef || !ef->fndef->ftype->ret->is(boolty)) {
            DO_FATAL
        }
        // �Զ����� bool ��������
        auto fcall = new ASTFunctionCall(ef->fndef);
        fcall->addparam(cond);
        cond = fcall;
    }

    // �½� if �ڵ�
    ASTIf *astif = new ASTIf(cond);

    // if ��䣬��������
    astif->pthen = buildChildScope("", "if_then"); // build();

    // ��� else����������
    auto word = getWord();
    if (ISCHA("else")) {
        astif->pelse = buildChildScope("", "if_else"); // build();
    } else {
        prepareWord(word); // ��λ
    }


    // ���� if �ڵ�
    return astif;
}

/**
 * while ѭ���ṹ
 */
AST* Build::build_while()
{
#define DO_FATAL FATAL("No match bool function Type<"+idn+"> when be used as a while flow !")
    
    AST* cond = build();
    Type* cty = cond->getType();
    Type* boolty = Type::get("Bool");

    string idn = cty->getIdentify();

    bool isboolty = cty->is(boolty);
    if ( ! isboolty) { // �� Bool ���ͣ����� bool ����
        auto *bools = dynamic_cast<ElementGroup*>(stack->find("bool"));
        if (!bools) { 
            DO_FATAL
        }
        auto fd = bools->elms.find("bool" DEF_SPLITFIX_FUNCARGV + idn);
        if (fd == bools->elms.end()) {
            DO_FATAL
        }
        auto *ef = dynamic_cast<ElementFunction*>(fd->second);
        if (!ef || !ef->fndef->ftype->ret->is(boolty)) {
            DO_FATAL
        }
        // �Զ����� bool ��������
        auto fcall = new ASTFunctionCall(ef->fndef);
        fcall->addparam(cond);
        cond = fcall;
    }

    // �½� while �ڵ�
    ASTWhile *astwhile = new ASTWhile(cond);

    // while ��䣬��������
    astwhile->body = buildChildScope("", "while"); //build();

    // ���� if �ڵ� 
    return astwhile;

}

/**
 * let ���Ű�
 */
AST* Build::build_let()
{
    auto *let = new ElementLet();
    auto *relet = new ASTLet();

    auto word = getWord();
    
    // Ψһ����
    string idname("");
    bool sign = false;

    // �������
    if (ISWS(Character)) {
        idname = word.value;
        relet->head.push_back(idname);
        // �������
        Word word;
        CHECKLPAREN("let macro binding need a sign ( to belong !")
        // ����
        while (true) {
            word = getWord();
            if (ISSIGN(")")) {
                break; // ��������
            }
            string str(word.value);
            let->params.push_back(str);
            relet->head.push_back(str);
        }

    // ��������
    } else if (ISSIGN("(")) {
        
        // ����
        while (true) {
            word = getWord();
            if (ISSIGN(")")) {
                break; // ��������
            }
            string str(word.value);
            relet->head.push_back(str);
            if (ISWS(Character)) {
                let->params.push_back(str);
                idname += DEF_SPLITFIX_OPRT_BIND;
            } else if (ISWS(Operator)) {
                idname += str;
                sign = true;
            } else {
                FATAL("let unsupported the symbol type '"+str+"' !")
            }
        }
        
        if (!sign || relet->head.size()<2
            || idname.substr(0,1)!=DEF_SPLITFIX_OPRT_BIND 
        ) { // �����ṩһ������ �Ҳ���Ϊǰ׺����
            FATAL("let operator binding format error !")
        }

    } 
   
    auto *fd = stack->find(idname, false);
    if (fd) { // �����ظ���
        FATAL("let can't repeat binding '"+idname+"' !")
    }
    
    // �������
    CHECKLPAREN("let binding need a sign ( to belong !")

    // ����
    list<Word> bodys;
    cacheWordSegment(bodys);
    for (auto &i : bodys) {
        let->bodywords.push_back(i);
        relet->body.push_back(i.value);
    }

    // ���ջ����
    stack->put(idname, let);

    // ����
    return relet;
}

/**
 * member call ��Ա��������
 */
AST* Build::build_elmivk()
{
    TypeStruct* tyclass(nullptr);
    AST* val(nullptr);

    auto word = getWord();

     // ͨ���������Ƶ��þ�̬��Ա����
    bool is_static = false;
    if (auto *e = dynamic_cast<ElementType*>(
        stack->find(word.value))) {
        if (tyclass = dynamic_cast<TypeStruct*>(e->type)) {
            is_static = true;
        }
    }
    // ͨ�������Ƶ��þ�̬����
    if ( ! tyclass) {
        prepareWord(word);
        val = build();
        // varibale = dynamic_cast<ASTVariable*>(val);
        if( ! val){
            FATAL("elmivk must belong a struct value !")
        }
        // val->print();
        tyclass  = validTypeStruct(val);
    }

    if( ! tyclass){
        FATAL("elmivk must belong a struct value '"+word.value+"' !")
    }

    // ���ڲ�ջ
    auto target_stack = type_member_stack[tyclass];

    // ��Ա��������
    word = getWord();
    string fname = word.value;

    // ��������
    ASTFunctionCall* fncall =
        _functionCall(fname ,target_stack, false);

    if ( ! fncall) {
        FATAL("can't find member function '"
            +fname+"' in class '"+tyclass->name+"'")
    }


    // ��̬��Ա������֤
    if (is_static && ! fncall->fndef->is_static_member) {
        FATAL("'"+fncall->fndef->ftype->name+"' is not a static member function !")
    }

    auto * mfc = new ASTMemberFunctionCall(val, fncall);

    return mfc;

}

/**
 * member get ��Ա����
 */
AST* Build::build_elmget()
{
    auto *ins = new ASTMemberVisit();

    AST* sctval = build();
    auto* scty = validTypeStruct(sctval);
    if( ! scty ){
        FATAL("elmget must eat a Struct value !")
    }

    ins->instance = sctval;

    auto word = getWord();
    int pos;
    if (ISWS(Number)) {
        if(Tokenizer::isfloat(word.value)){
            FATAL("elmget index can't be a float !")
        }
        pos = Str::s2l(word.value);
    } else {
        pos = scty->elmpos(word.value);
    }

    // �������
    if (pos<0 || pos>scty->len()) {
        FATAL("class '"+scty->name+"' no element '" + word.value + "' !")
    }
    ins->index = pos;

    // �Ƿ�Ϊȡ����ֵ
    if (auto * qty = dynamic_cast<TypeRefer*>(scty->types[pos])) {
        if (!qty->type) { // ��ʼ��֮ǰ����ʹ��
            FATAL("Can't use quote value "+scty->name+"."+word.value+" before initialize !")
        }
        return new ASTLoad(ins, qty); // ��������
    }

    return ins;
}

/**
 * member set ��Ա��ֵ
 */
AST* Build::build_elmset()
{
    auto *ins = new ASTMemberAssign();

    AST* sctval = build();
    auto* scty = validTypeStruct(sctval);
    if( ! scty ){
        FATAL("elmset must eat a Struct value !")
    }

    ins->instance = sctval;

    auto word = getWord();
    int pos;
    if (ISWS(Number)) {
        pos = Str::s2l(word.value);
    } else {
        pos = scty->elmpos(word.value);
    }

    // �������
    if (pos<0 || pos>scty->len()) {
        FATAL("class '"+scty->name+"' no element '" + word.value + "' !")
    }
    ins->index = pos;
    
    AST* putv = build();
    Type* putty = putv->getType();
    // ���ͼ��
    Type* pvty = scty->types[ins->index];
    if (auto * qty = dynamic_cast<TypeRefer*>(pvty)) {
        // ��������
        if (qty->type && !putty->is(qty->type)) {
            FATAL("can't quote member assign <"+pvty->getIdentify()+"> by <"+putty->getIdentify()+">' !")
        }
        // ��һ�θ�ֵ�̶�����
        if (!qty->type) {
            qty->type = putty;
        }
        // ȡ������
        putv = new ASTQuote(putv, qty);

    } else if (!putty->is(pvty)) {
        FATAL("can't member assign <"+pvty->getIdentify()+"> by <"+putty->getIdentify()+">' !")
    }

    ins->value = putv;

    return ins;
}

/**
 * member function �ⲿ����
 */
AST* Build::build_elmdef()
{
    auto word = getWord();
    // ��������ֵ����
    TypeStruct *ty(nullptr);
    if(auto *e=dynamic_cast<ElementType*>(stack->find(word.value))){
        if (auto *y = dynamic_cast<TypeStruct*>(e->type)) {
            ty = y;
        }
    } 
    // ����Ϊ����
    if (NOTWS(Character) || ! ty) {
        FATAL("member function external define need a type name to belong !")
    }

    // �滻����ջ
    Stack* old_stack = stack;
    stack = type_member_stack[ty];
    stack->tydef = type_define[ty];

    // ����
    AST* res = build();

    // ��λջ֡
    stack = old_stack;

    return new ASTExternalMemberFunctionDefine(ty, res);
}

/**
 * Multiple macro _ ռλ�� �ظ���
 */
AST* Build::build_mcrfor()
{
    // �������
    Word word;
    CHECKLPAREN("multiple macro need a sign ( to belong !")

    // �ظ������
    list<list<Word>> argvs;
    while (true) {
        list<Word> item;
        word = getWord();
        if (ISSIGN(")")) {
            break; // ��������
        } else if (ISSIGN("(")) {
            cacheWordSegment(item);
        } else {
            item.push_back(word);
        }
        argvs.push_back(item);
    }
    
    // �������
    CHECKLPAREN("multiple macro need a sign ( to belong !")

    // �ظ�����
    list<Word> bodys;
    while (true) {
        word = getWord();
        if (ISSIGN(")")) {
            break; // ��������
        }
        bodys.push_back(word);
    }
    
    // ��������
    list<Word> prepares;
    size_t i(0);
    for (auto & a : argvs) {
        for (auto & word : bodys) {
            if (ISCHA("_v")) { // ֵ
                for(auto &w : a){
                    prepares.push_back(w);
                }
            } else if (ISCHA("_i")){ // ����
                prepares.push_back(Word(Tokenizer::State::Number,Str::l2s(i)));
            } else {
                prepares.push_back(word);
            }
        }
        i++;
    }

    // Ԥ��
    prepareWord(prepares);
    
    // ���Դ�ӡ
    DEBUG_WITH("mulmcr_words", \
        if(prepares.size()){ \
        cout << "mulmcr��"; \
        for (auto &p : prepares) { \
            cout << " " << p.value; \
        } \
        cout << "��" << endl; \
        } \
        )


    // ��������
    return build();

}

/**
 *  macro if ������չ��
 */
AST* Build::build_mcrif()
{
    Word word;
    auto word1 = getWord();
    auto word2 = getWord();
    
    // �������
    CHECKLPAREN("condition macro need a sign ( to belong !")

    // ����
    list<Word> bodys;
    cacheWordSegment(bodys);

    // �ж������Ƿ�һ��
    if (word1==word2) {
        prepareWord(bodys);
    }

    // ���½���
    return build();

}

/**
 *  macro cut ��ֶ�
 * ���ݲ����ã�������
 */
AST* Build::build_mcrcut()
{
    Word word = getWord();

    // cout << "����������" << word.value << endl;
    
    // ����Ϊ����
    if (NOTWS(Number)) {
        FATAL("macro cut need a <Number> belong !")
    }

    size_t seg = Str::s2l(word.value);

    // ����
    auto Lc = Word(State::Sign, "(");
    auto Rc = Word(State::Sign, ")");
    prepareWord(Lc); // (

    size_t i(0);
    while (true) {
        word = getWord();
        if (ISSIGN(")")) {
            Rc = word;
            break;
        }
        if (0==i%seg) {
            prepareWord(Rc);
            prepareWord(Lc);
        }
        prepareWord(word);
        i++;
    }
    prepareWord(Rc); // )


}

/**
 *  macro link ������������
 */
AST* Build::build_mcrlnk()
{
    Word word1 = getWord();
    Word word2 = getWord();
    word1.value += word2.value;
    prepareWord(word1);

    return build();
}

/**
 * array �½��������Ͷ���
 */
AST* Build::build_refer()
{
    // ֻ�������Ա��������ʹ����������
    FATAL("only in class or array can use reference type !")
}

/**
 * array �½��������Ͷ���
 */
AST* Build::build_array()
{
    Word word = getWord();
    // �����С ����Ϊ����
    if (NOTWS(Number)) {
        FATAL("array define need a <Number> belong !")
    }
    size_t len = Str::s2l(word.value);
    
    // Ԫ������
    word = getWord();

    if (NOTWS(Character)) {
        FATAL("array define need a legal type name to belong !")
    }
    
    // ���Ϊ��������
    bool is_ref = false;
    if(ISCHA(DEF_TYPE_KEYWORW_REFERENCE)) {
        is_ref = true;
        word = getWord();
    }

    // ����
    Type *ty(nullptr);
    
    Element* res = stack->find(word.value);
    if (ElementType* dco = dynamic_cast<ElementType*>(res)) {
        ty = dco->type;
        if(is_ref){
            ty = new TypeRefer(ty);
        }
    } else {
        FATAL("array define error: not find Type <"<<word.value<<"> !")
    }

    // ��������

    auto *aryty = new TypeArray(ty, len);

    // �������鹹��
    return new ASTArrayConstruct(aryty);

}

/**
 * tuple �½�Ԫ�����Ͷ���
 */
AST* Build::build_tuple()
{
    Word word; //
    
    // �������
    CHECKLPAREN("tuple define need a sign ( to belong !")
        
    // �½�Ԫ�����ͣ�������
    auto *tupty = new TypeStruct("");
    auto *tupval = new ASTTypeConstruct(tupty);

    // Ԫ��Ԫ��
    while(true){
        AST* li = build();
        if (nullptr==li) {
            break;
        }
        tupval->add(li);
        tupty->add(li->getType());
    }
    
    // �������
    CHECKRPAREN("tuple define need a sign ) to belong !")
        
    // ����Ԫ��ֵ
    return tupval;

}

/**
 * array �����Ա����
 */
AST* Build::build_arrget()
{
    // �������
    AST *ary = build();

    // ���ͼ��
    TypeArray * arty(nullptr);
    if ( ! (arty=validTypeArray(ary))) {
        FATAL("array visit must use to array type !")
    }
    
    // ����
    AST *idx = build();

    // ���ͼ��
    if (!dynamic_cast<TypeInt*>(idx->getType())) {
        FATAL("array visit must get a Number type index !")
    }
    
    // ����Ԫ�ط���
    AST* visit = new ASTArrayVisit(ary, idx);

    // �������������
    if (auto *refty = dynamic_cast<TypeRefer*>(arty->type)) {
        // ��������
        visit = new ASTLoad(visit, refty);
    }

    return visit;
}

/**
 * array �����Ա��ֵ
 */
AST* Build::build_arrset()
{
    // �������
    AST *ary = build();

    // ���ͼ��
    TypeArray * arty(nullptr);
    if (! (arty=validTypeArray(ary))) {
        FATAL("array assign must use to array type !")
    }
    
    // ����
    AST *idx = build();

    // ���ͼ��
    if (!dynamic_cast<TypeInt*>(idx->getType())) {
        FATAL("array assign must get a Number type index !")
    }
    
    // ֵ
    AST *value = build();
    Type* vty = value->getType();


    // �������������
    if (auto *refty = dynamic_cast<TypeRefer*>(arty->type)) {
        // ȡ�����õ�ֵַ
        Type* tarty = refty->type;
        if (! vty->is(tarty)) {
            FATAL("can't quote member assign <"
                +tarty->getIdentify()+"> by <"+vty->getIdentify()+">' !")
        }
        value = new ASTQuote(value, refty);
        
    // ���ͼ��
    } else if (! vty->is(arty->type)) {
        FATAL("can't quote member assign <"
            +arty->getIdentify()+"> by <"+vty->getIdentify()+">' !")
    }
    
    // ��������Ԫ�ظ�ֵ
    return new ASTArrayAssign(ary, idx, value);
}

/**
 * adt ������ģʽ
 */
AST* Build::build_adt()
{
    setModADT(true);
    return build();
}

/**
 * new ���ڴ�����
 */
AST* Build::build_new()
{
    Word word = getWord();
    
    AST* new_len(nullptr);
    Type* intty = Type::get("Int");

    // �������������
    if (ISCHA(DEF_TYPE_KEYWORW_ARRAY)) {
        new_len = build();
        if ( ! new_len->getType()->is(intty)) {
            FATAL("new array length must be a Int type !")
        }
        Type* scty = expectTypeState();
        // �����ڴ�
        auto *vptr = new ASTMalloc(scty, new_len);
        vptr->is_array = true;
        // ����
        return new ASTNew(vptr);
    }
     
    // ��ͨ����ռ� 
    new_len = new ASTConstant(intty, "1");
   
    // ����
    // word = getWord();
    ElementType* ety(nullptr);
    if (NOTWS(Character) || !(ety=dynamic_cast<ElementType*>(stack->find(word.value, true)))) {
        FATAL("new format error !" + Str::l2s((int)ety))
    }
    
    TypeStruct* scty(nullptr);
    if (! (scty=dynamic_cast<TypeStruct*>(ety->type))) {
        FATAL("new object type error !")
    }
    // �����ڴ�
    AST * vptr = new ASTMalloc(scty, new_len);

    // ���ù��캯��
    AST * cons = buildConstruct(scty, vptr);

    // ����
    return new ASTNew(cons);
}

/**
 * delete ���ڴ��ͷ�
 */
AST* Build::build_delete()
{
    AST *del = build();
    Type *ty = del->getType();

    // del->print();
    // cout << del->getType()->str() << endl;

    if (auto*ld=dynamic_cast<ASTLoad*>(del)) {
    } else if ( ! dynamic_cast<TypePointer*>(del->getType())) {
        FATAL("Can't delete a no new object !")
    }
        // del = new ASTQuote(del, ty);

    return new ASTDelete(del);
}

/**
 * copy ��������
 */
AST* Build::build_copy()
{
    AST *obj = build();
    
    // ���ͼ��
    if (!dynamic_cast<TypeStruct*>(obj->getType())) {
        FATAL("copy must use to a Class type !")
    }

    // ����
    return new ASTCopy(obj);
}

/**
 * ȫ��Ψһ���� ����
 */
AST* Build::build_uvnnew()
{
    auto word = getWord();
    string key = word.value;
    
    // ����Ƿ��ض���
    auto it = uq_var_names.find(key);
    if(it != uq_var_names.end()){
        FATAL("Repeated initializat unique variable name : "+key)
    }

    // ������
    uq_var_autoincrement++;
    string uvn =  "%" + key + Str::l2s(uq_var_autoincrement);
    uq_var_names[key] = uvn;

    // ���� AST
    return new ASTUVNnew(key, uvn);
}

/**
 * ȫ��Ψһ���� ��ȡ
 */
AST* Build::build_uvnget()
{
    // ��ȥΨһ����
    Word wd = getUniqueVariableName(getWord().value);

    // ����
    prepareWord(wd);

    // ���¿�ʼ
    return build();
}

/**
 * ȫ��Ψһ���� ɾ��
 */
AST* Build::build_uvndel()
{
    auto word = getWord();
    string key = word.value;
    
    // ����Ƿ��ض���
    auto it = uq_var_names.find(key);
    if(it == uq_var_names.end()){
        FATAL("Cannot to delete not exist unique variable name : "+key)
    }

    // ɾ��
    uq_var_names.erase(it);

    return new ASTUVNdel(key);
}

/**
 * ȫ��Ψһ���� ���
 */
AST* Build::build_uvnclear()
{
    uq_var_names.clear();

    return new ASTUVNclear();
}

/**
 * scope �ֲ����������ֿռ�����
 */
AST* Build::build_scope()
{
    Word word = getWord();
    string space = "";

    // ���ֿռ�
    if(ISWS(Character)){
        space = word.value;
    } else {
        prepareWord(word);
    }

    // ����
    return buildChildScope(space);
}

/**
 * uscp �������ֿռ�
 */
AST* Build::build_scpuse()
{
    string name("");
    list<Word> wds;
    cacheWordCell(wds, true);
    for(auto w : wds){
        _useScope(w.value);
        name += " " + w.value;
    }
    name[0] = '(';

    // ����
    return new ASTUseScope(name+")");
}

/**
 * build_delscp ж�����ֿռ�
 */
AST* Build::build_scpdel()
{
    string name("");
    list<Word> wds;
    cacheWordCell(wds, true);
    for(auto w : wds){
        _delScope(w.value);
        name += " " + w.value;
    }
    name[0] = '(';
    // ����
    return new ASTDeleteScope(name+")");
}


/**
 * link �õ�����������
 *
AST* Build::build_link()
{
    AST* value = build();
    return new ASTQuote(value);
}

/**
 * load �����õõ�����
 *
AST* Build::build_load()
{
    AST* value = build();
    return new ASTLoad(value);
}

*/



/********************************************************/


/**
 * Ԥ���Ƿ���Ҫ�⿪���Ű�
 */
bool Build::forecastOperatorBind()
{
    bool spread = false; // �Ƿ���չ
    bool fail = false;
    list<Word> cache;
    while (true) {
        auto word = getWord();
        if (ISWS(End)) { // ����
            break; // ����
        }
        cache.push_back(word);
        if (ISSIGN("(")) { // �Ӽ�
            if (fail) break;
            cacheWordSegment(cache, false); // �������Ŷ�����
            fail = true;
            continue;
        }
        if (ISWS(Operator)) {
            spread = true; // ��Ҫ���
            break;
        }
        if (ISWS(Sign)) { // ��������
            break;
        }
        if (fail) {
            break; // ��
        }
        fail = true;
    }
    
    /*
    cout << "cache��";
    for (auto &p : cache) {
        cout << " " << p.value;
    }
    cout << "��" << endl;
    */

    // �ָ�
    prepareWord(cache);

    return spread;
}


/**
 * let �������󶨰�չ��
 */
AST* Build::buildOperatorBind()
{
    //��չ������
    if (! forecastOperatorBind()) {
        return nullptr; 
    }

    // չ����
    auto words = spreadOperatorBind();
    prepareWord(words); // չ����չ�������

    // ���ٴμ���Ƿ���չ
    return build(false);
}


/**
 * let �������󶨰�չ��
 */
list<Word> Build::spreadOperatorBind(list<Word>*pwds)
{
    string idname("");
    vector<list<Word>> params;

    if (pwds) { // ������
        idname = DEF_SPLITFIX_OPRT_BIND;
        params.push_back(*pwds);
    }

    ElementLet* let(nullptr);
    filterLet* filter(nullptr);
    bool end = false;
    list<Word> cache;
    while (true) {
        cache.clear();
        auto word = getWord();
        if (ISWS(End)) { // �ı�����
            end = true;
        }
        cache.push_back(word);
        if (ISSIGN("(")) { // �Ӽ�
            cacheWordSegment(cache, false); // �������Ŷ�����
            if (cache.empty()) {
                FATAL("Operator binding priority error !")
            }
            params.push_back(cache);
            idname += DEF_SPLITFIX_OPRT_BIND;
        }else if (ISWS(Operator)) { // ������
            idname += word.value;
        } else {
            idname += DEF_SPLITFIX_OPRT_BIND;
            list<Word> pms;
            pms.push_back(word);
            params.push_back(pms);
        }
        if ( ! filter) { // ��ʼ��ɸѡ��
            filter = new filterLet(stack, idname);
        }
        // ɸѡƥ��
        int match = filter->match(idname);
        if (match==1 && filter->unique) { // ̰��ƥ�����
            let = filter->unique;
            break; // �ҵ�Ψһƥ��
        }
        if(end||match==0){
            if (let) {
                prepareWord(cache); // ��λ��������
                break; // ������һ����ȫƥ��
            }
            FATAL("can't find the binding '"+idname+"' !")
        }
        // ��¼��һ����Ψһƥ��
        let = filter->unique;
    }

    delete filter;

    // ��������
    map<string, list<Word>> pmstk;
    int i = 0;
    for (auto &p : let->params) {
        pmstk[p] = params[i];
        i++;
    }

    // չ����
    list<Word> results;
    for (auto &word : let->bodywords) {
        if (ISWS(Character) && pmstk.find(word.value) != pmstk.end()) {
            // �⿪����
            auto wd = pmstk[word.value];
            for(auto &w : wd){
                results.push_back(w);
            }
        }else {
            results.push_back(word);
        }
    } 

    // ���Դ�ӡ
    DEBUG_WITH("binding_spread", \
        if(results.size()){ \
        cout << "binding spread "+idname+"��"; \
        for (auto &p : results) { \
            cout << " " << p.value; \
        } \
        cout << "��" << endl; \
        } \
    )

    // �ж���һ���Ƿ�Ϊ���Ű�
    auto word = getWord();
    prepareWord(word);
    if (ISWS(Operator)){
        return spreadOperatorBind(&results);/*
        filterLet filter(stack, DEF_SPLITFIX_OPRT_BIND + word.value);
        if (filter.size()>0) { // ��ѯƥ��
            return spreadOperatorBind(&results);
        }*/
    }


    // ���Ű󶨽���
    return results;
}


/********************************************************/


/**
 * ���浥�ʶΣ����������ڲ��������ݣ�
 */
void Build::cacheWordSegment(list<Word>& cache, bool strip)
{
    int down = 1;
    while (true) {
        auto word = getWord();
        if (ISWS(End)) { // ����
            return;
        }
        cache.push_back(word);
        if (ISSIGN("(")) down++;
        if (ISSIGN(")")) down--;
        if (0 == down) break;
    }
    if (strip) {
        cache.pop_back(); // ȥ��β������
    }
}
void Build::cacheWordCell(list<Tokenizer::Word>& wds, bool strip)
{
    Word word = getWord();
    if(ISSIGN("(")){
        if(!strip) wds.push_back(word);
        cacheWordSegment(wds, strip); // ��Ҫ����
    } else {
        wds.push_back(word);
    }
}

/**
 * ������ͱ�ע
 */
def::core::Type* Build::expectTypeState()
{
    // ������������ͱ��
    string mark = "";
    std::stack<int> array_sizes;

    list<Word> caches;

    Word word;

#define READWORD word = getWord(); caches.push_back(word);
#define RETURNNULL prepareWord(caches); return nullptr;

    // ������ͱ��
    while (true) {
        READWORD
        if (NOTWS(Character)) {
            RETURNNULL
        }
        if (ISCHA(DEF_TYPE_KEYWORW_REFERENCE)) {
            mark += "r";
        } else if (ISCHA(DEF_TYPE_KEYWORW_ARRAY)) {
            mark += "a";
            Word word = getWord();
            if (ISWS(Number)) { // �Ƿ���������С
                array_sizes.push(Str::s2l(word.value));
            } else {
                array_sizes.push(0);
                prepareWord(word);
            }
        } else {
            break;
        }
    }

    // ��������
    Type *ty(nullptr);
    Element* res = stack->find(word.value);
    // ��ͨ����
    if (auto *dco = dynamic_cast<ElementType*>(res)) {
        ty = dco->type;
    // ģ������
    } else if (auto *ttd = dynamic_cast<ElementTemplateType*>(res)) {
        ty = _templateType(ttd->tptydef);
    } else {
        RETURNNULL
    }

    // �������
    int len = mark.size();
    while (len--) {
        char m = mark[len];
        if (m=='a') {
            ty = new TypeArray(ty, array_sizes.top());
            array_sizes.pop();
        } else if (m=='r') {
            ty = new TypeRefer(ty);
        }
    }

    return ty;


#undef READWORD 
#undef RETURNNULL 

}

/**
 * �����Ч�ı�ʾ������
 */
Word Build::expectIdName(const string & error)
{
    Word word = getWord();
    
    if (NOTWS(Character)) {
        FATAL(error)
    }

    // ������
    if (word.value == "mcrlnk") {
        Word word1 = getWord();
        Word word2 = getWord();
        word1.value += word2.value;
        return word1;
    }

    // ȫ��Ψһ��
    if(word.value == "uvnget"){
        Word word1 = getWord();
        return getUniqueVariableName(word1.value);
    }

    return word;
}

/**
 * ȫ��Ψһ���ƻ�ȡ
 */
Word Build::getUniqueVariableName(const string & n)
{
    auto it = uq_var_names.find(n);
    if(it == uq_var_names.end()){
        FATAL("Not initialized unique variable name : "+n)
        // return "";
    }
    Word wd = Word(
        Tokenizer::State::Character, 
        it->second
    );
    return wd;
}



/********************************************************/



/**
 * ������������
 */
TypeFunction* Build::_functionType(bool declare)
{
    return nullptr;
}

/**
 * ������������
 */
ASTFunctionCall* Build::_functionCall(const string & fname, Stack* stack, bool up)
{
    auto *fncall = new ASTFunctionCall(nullptr);
    ASTFunctionDefine *fndef(nullptr);

    // �ڵ㻺��
    list<AST*> cachebuilds;

    // ��ʱ������ѯ����������
    auto *tmpfty = new TypeFunction(fname);
    filterFunction filter = filterFunction(stack, fname, up);
    if (!filter.size()) {
        return nullptr; // �޺���
    }

    // ����̰��ƥ��ģʽ
    while (true) {
        // ƥ�亯������һ���޲Σ�
        int match = filter.match(tmpfty);
        if (match==1 && filter.unique) {
            fndef = filter.unique; // �ҵ�Ψһƥ��
            string idname = fndef->ftype->getIdentify();
            break;
        }
        // ��ƥ��
        if (match==0) {
            prepareBuild(cachebuilds); // ��λ�����Ľڵ�
            delete tmpfty;
            delete fncall;
            return nullptr;
        }
        // �жϺ����Ƿ���ý���
        auto word = getWord();
        prepareWord(word); // �ϲ㴦������
        if (ISWS(End) || ISSIGN(")")) { // ��ǰ�ֶ����ý���
            if (filter.unique) {
                fndef = filter.unique; // �ҵ�Ψһƥ��
                break;
            }
        }
        // ��Ӳ���
        AST *exp = build();
        if (!exp) {
            if (filter.unique) {
                fndef = filter.unique; // �޸���������ҵ�ƥ��
                break;
            }
        }
        cachebuilds.push_back(exp);
        fncall->addparam(exp); // ��ʵ��
        auto *pty = validType(exp); // �Ϸ��Ĳ�������
        // ���غ�����
        tmpfty->add(pty); // ����
    }

    fncall->fndef = fndef; // elmfn->fndef;
       
    // �������ý����ɹ�
    delete tmpfty;
    return fncall;
}

/**
 * �Զ���ӷ���ֵ
 */
def::core::Type* Build::_autoAddFuncRet(ASTFunctionDefine* fndef)
{

    // ��鷵��ֵ����һ����
#define RETUTN verifyFunctionReturnType(tyret); return tyret;

    Type* tyret = Type::get("Nil");
    // ���캯���޷���ֵ
    if (fndef->is_construct) {
        RETUTN
    }
    // �ǿ�
    size_t len = fndef->body->childs.size();
    while (len--) {
        AST *last = fndef->body->childs[len];
        if ( !last->isCodegen()) {
            continue; // 
        }
        if (last->isValue()) {
            tyret = last->getType();
            // �Զ����� ret ���
            if ( ! dynamic_cast<ASTRet*>(last)) {
                // cur_fun_ret = last; // ���ؽڵ�
                fndef->body->childs[len] = new ASTRet(last);
            }
            RETUTN
        }
        break;
    }

    // ��� void ����ֵ
    fndef->body->add(new ASTRet(nullptr));
    // ��鷵��ֵ����һ����
    RETUTN
#undef RETUTN
}

/**
 * ����ģ��õ��µ�����
 */
TypeStruct* Build::_templateType(ASTTemplateTypeDefine* tptydef)
{
    // ������
    // TypeStruct* typety = new TypeStruct(name);
    
    // ����������
    // auto *tydef = new ASTTypeDefine();

    // auto * tptydef = tpty->tptydef;
    
    // ������������
    string tptyid("");
    string tptyname("");

    // ��ȡ��ģ��ʵ��
    // size_t arglen = tptydef->params.size();
    // ����
    map<string, Word> args;
    for(auto k : tptydef->params){
        auto word = getWord();
        // ����Ƿ�Ϊ����
        if(auto*elm=dynamic_cast<ElementType*>(stack->find(word.value))){
            args[k] = word;
            tptyname += "," + elm->type->getIdentify(); // ����
            tptyid += "," + Str::l2s((int)(elm->type)); // �ڴ�id
        } else{
            FATAL("Type template call need a valid type name !")
        }
    }
    tptyid[0] = '<';
    tptyname[0] = '<';
    tptyid = Str::l2s((int)(tptydef)) + tptyid + ">";
    tptyname = tptydef->name + tptyname + ">";

    // �����ģ�����ջ���Ƿ������ɵ�����
    if(auto *elmty = dynamic_cast<ElementType*>(tpty_stk->find(tptyid))){
        // ������������
        // cout << "_templateType cache: " << tptyid << endl;
        return (TypeStruct*)elmty->type;
    }
    

    // ������ģ�����ģ�ʵ���滻
    list<Word> body;
    body.push_back(Word(Tokenizer::State::Character, tptyname));
    body.push_back(Word(Tokenizer::State::Sign,"("));
    for (auto &word : tptydef->bodywords) {
        if(NOTWS(Character)){
            body.push_back(word);
            continue;
        }
        auto fd = args.find(word.value);
        if (fd == args.end()) {
            body.push_back(word);
            continue;
        }
        // �滻
        body.push_back(fd->second);
    }

    // Ԥ��
    prepareWord(body);
    

    // �趨ģ��״̬
    tpl_ty_name = tptydef->name;
    // ��������������ģ��ջ
    auto ty = ((ASTTypeDefine*)build_type())->type;
    // ���״̬
    tpl_ty_name = "";
    
    // ��ӵ���ģ��ջ������ 
    tpty_stk->put(tptyid, new ElementType(ty));

    // ��������
    return ty;
}


/**
 * ������ж�����ֿռ�
 */
bool Build::_useScope(const string & n)
{
    string name = n;
    // ��ʼ���ҷ���ջ
    Stack * stk = stack->use(name);
    if(!stk){
        FATAL("Cannot find the namespace '"+name+"' !")
    }
    // ��ӵ�ʹ�õķ���ջ
    // �����ӵ�ջ������ʹ��
    stack->uscps.push_front(make_tuple(name, stk));
    return true;
}
bool Build::_delScope(const string & n)
{
    string name = n;
    // ����
    for(auto one : stack->uscps){
        if(std::get<0>(one)==name){
            // ���ҳɹ���ж��
            stack->uscps.remove(one);
            return true;
        }
    }
    FATAL("Cannot Cancer the not existence namespace '"+name+"' !")
}






