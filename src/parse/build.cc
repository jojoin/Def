/**
 * 语法分析空间
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

// 检查括号
#define CHECKLPAREN(T) word = getWord(); \
    if (NOTSIGN("(")) { FATAL(T) }
#define CHECKRPAREN(T) word = getWord(); \
    if (NOTSIGN(")")) { FATAL(T) }

    
/**
 * 构造
 */
Build::Build(Tokenizer * t)
    : Service(t)
{
}

    
/**
 * 缓存或预备待分析节点
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
 * 启动分析
 */
ASTGroup* Build::createAST()
{
    return buildGroup();
}

/**
 * 创建 Group
 */
ASTGroup* Build::buildGroup()
{
    // 新建抽象树
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
 * 创建表达式
 * spread 是否需要检测 let 符号绑定
 */
AST* Build::build(bool spread)
{
    
    // 调试打印
    DEBUG_WITH("prepare_words", \
        if(prepare_words.size()){ \
        cout << "prepare【"; \
        for (auto &p : prepare_words) { \
            cout << " " << p.value; \
        } \
        cout << "】" << endl; \
        } \
        )


    if (!prepare_builds.empty()) {
        auto rt = prepare_builds.front();
        prepare_builds.pop_front();
        return rt;
    }

    // 检查符号展开
    if (spread) {
        if (AST* res = buildOperatorBind()) {
            return res;
        }
    }

    Error::snapshot();

    Word word = getWord();
    
    if ( ISWS(End) ) {
        return nullptr; // 词法扫描完成
    }


    // 语句结束
    if (ISSIGN(";")) {
        return build();
        
    // group
    } else if(ISSIGN("(")){
        //cout << "create new scope !" << endl;
        // 新建分析栈
        //auto * old_stack = stack;
        //stack = new Stack(old_stack);
        ASTGroup* grp = buildGroup();
        // 复位分析栈
        //delete stack;
        //stack = old_stack;
        // 结束验证
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

        // 查询标识符是否定义
        Element* res = stack->find(chara, true);
        
        // 变量请求 ？
        if (auto val = buildVaribale(res, chara)) {
            return val;
        }

        // 宏绑定 ？
        if (auto gr = dynamic_cast<ElementLet*>(res)) {
            return buildMacro(gr, chara);
        }
        
        // 类型构造 ？
        if (auto gr = dynamic_cast<ElementType*>(res)) {
            return buildConstruct((TypeStruct*)gr->type);
        }

        // 函数调用 ？
        if (auto gr = dynamic_cast<ElementGroup*>(res)) {
            auto fncall = _functionCall(chara ,stack);
            if (fncall) {
                return fncall;
            }
        }
        
        // 类模板调用？
        if(auto gr = dynamic_cast<ElementTemplateType*>(res)){
            return buildTemplateType(chara, gr);
        }

        // 函数模板调用？
        res = stack->find(DEF_PREFIX_TPF+chara, true);
        if (res) {
            return buildTemplateFuntion(chara, (ElementTemplateFuntion*)res);
        }

        // 语言核心定义？
        AST *core = buildCoreDefine(chara);
        if (core) {
            return core;
        }

        // 为 Nil 类型字面常量
        if (chara=="nil") {
            return new ASTConstant(Type::get("Nil"), word.value);
        }

        // 为 Bool 类型字面常量
        if (chara=="true"||chara=="false") {
            return new ASTConstant(Type::get("Bool"), word.value);
        }

        // 错误
        Error::exit("Undefined identifier: " + chara);
    }
    

    Error::backspace(); // 复位

    // 缓存单词  留待上层处理
    // cacheWord(word);
    prepareWord(word);

    return nullptr; // 词法扫描完成


}
    

/********************************************************/


/**
 * 变量或类成员访问
 */
AST* Build::buildVaribale(Element* elm, const string & name)
{
    if (auto gr = dynamic_cast<ElementVariable*>(elm)) {
        auto *val = new ASTVariable( name, gr->type );
        val->origin = gr->origin;
        // 全局唯一名称
        val->unique_name = gr->unique_name;
        return val;
    }

    // 是否为类成员访问
    if(stack->tydef && stack->fndef){
        AST* instance = new ASTVariable( // 类变量
            DEF_MEMFUNC_ISTC_PARAM_NAME,
            stack->tydef->type
        );
        // 访问类本身
        if (name==DEF_MEMFUNC_ISTC_PARAM_NAME) {
            stack->fndef->is_static_member = false; // 有成员函数
            return instance;
        }
        int i = 0;
        for (auto &p : stack->tydef->type->tabs) {
            if (p==name) { // 找到
                // 返回类成员访问
                AST* elmget = new ASTMemberVisit(instance, i);
                stack->fndef->is_static_member = false; // 有成员函数
                // 是否为取引用值
                if (auto * qty = dynamic_cast<TypeRefer*>(stack->tydef->type->types[i])) {
                    if (!qty->type) { // 初始化之前不能使用
                        FATAL("Can't use quote value before initialize !")
                    }
                    return new ASTLoad(elmget, qty); // 引用载入
                }
                return elmget;
            }
            i++;
        }
    }


    return nullptr;
}

/**
 * 核心定义处理
 */
AST* Build::buildCoreDefine(const string & name)
{
    /** def 核心定义列表 **/
#define T(N) if(#N==name) return build_##N();
    BUILTIN_DEFINES_LIST(T)
#undef T

    // 未匹配
    return nullptr;

}

/**
 * 解析模板函数
 */
AST* Build::buildTemplateFuntion(const string & name, ElementTemplateFuntion* tpf)
{
    // 查询 tpf_stk 函数模板栈缓存
    string tpfid("");
    // 新函数类型
    TypeFunction* functy = new TypeFunction(name);
    // 创建新函数
    auto *fndef = new ASTFunctionDefine(functy);
    // 创建模板函数调用
    auto *fncall = new ASTFunctionCall(fndef);
    // 创建新分析栈
    Stack* old_stack = stack;
    auto * new_stack = new Stack(stack, Stack::Mod::Function); // 函数分析栈
    // 实参入栈
    for (auto &pn : tpf->tpfdef->params) {
        AST* p = build();
        fncall->addparam(p);
        Type *ty = p->getType();
        functy->add(ty, pn); // 参数类型
        new_stack->put(pn, new ElementVariable(ty)); // 加实参
        // 缓存key
        tpfid += "," + Str::l2s((int)ty);
    }
    tpfid[0] = '(';
    tpfid = Str::l2s((int)tpf->tpfdef) + tpfid + ")";


    // 查询缓存
    if(auto elmfdef=dynamic_cast<ElementFunction*>(tpf_stk->find(tpfid))){
        // 无需重新生成
        // cout << "return buildTemplateFuntion cache: " << tpfid << endl;
        delete functy;
        delete fndef;
        fncall->fndef = elmfdef->fndef;
        // 缓存到当前栈
        stack->addFunction(elmfdef->fndef);
        return fncall;
    }
    
    // 新建函数定义
    fndef->wrap = stack->fndef; // wrap
    new_stack->fndef = fndef; // 当前定义的函数
    // 替换新栈帧
    stack = new_stack;
    // 预备函数体词组
    prepareWord(tpf->tpfdef->bodywords);
    // 解析函数体
    ASTGroup *body = createAST();
    auto word = getWord(); 
    if(NOTSIGN(")")){
        FATAL("Error format function body !)")
    } 
    // 加上 Body
    fndef->body = body;
    // 自动添加返回值语句， 检查返回值类型一致性
    Type* tyret = _autoAddFuncRet(fndef);
    // 设置可能标记的返回值
    functy->ret = tyret;
    fndef->ftype = functy;
    // 复位旧栈帧
    stack = old_stack;
    // 添加新函数
    stack->addFunction(fndef);
    // 添加到 tpf_stk 函数模板缓存栈
    // cout << "put cache: " << tpfid << endl;
    tpf_stk->put(tpfid, new ElementFunction(fndef));
    // 返回函数调用
    return fncall;
}

/**
 * 解析模板类
 */
AST* Build::buildTemplateType(const string & name, ElementTemplateType* tpty)
{
    // 获取类型
    TypeStruct* claty = _templateType(tpty->tptydef);

    // 类型初始化
    return buildConstruct(claty);
}

/**
 * 变量构造函数调用
 */
AST* Build::buildConstruct(TypeStruct* tyclass, AST* vptr)
{
    string name = tyclass->name;

    // 构造函数名称
    string fname = DEF_PREFIX_CSTC + name;

    // auto *tyclass = (TypeStruct*)ety->type;
    // 查找是否存在构造函数
    auto fd = stack->find(fname);

    // 无构造函数时
    if (!fd) {

        auto *cst = new ASTTypeConstruct(tyclass);
        cst->instance = vptr; // 堆内存
        int plen = tyclass->len();
        while (plen--) { // 添加类型构造参数
            cst->add(build());
        }
        return cst;
    }

    // 使用已分配的内存 或 调用空构造函数
    auto *val = vptr ? vptr : new ASTTypeConstruct(tyclass, true); // 空构造

    // 类型内部栈
    auto target_stack = type_member_stack[tyclass];

    // 函数调用
    ASTFunctionCall* fncall =
        _functionCall(fname ,target_stack, false);

    // 未找到合适的构造函数
    if ( ! fncall) {
        FATAL("can't match class '"+tyclass->name
            +"' construct function '"
            +fname+"' !")
    }


    /*
    // 查询目标类成员函数
    // auto word = getWord();
    auto target_stack = type_member_stack[tyclass];
    // 成员函数调用，不向上查找
    auto filter = filterFunction(target_stack, funname, false);
    if (!filter.size()) {
        FATAL("can't find member function '"
            +funname+"' in class '"+tyclass->name+"'")
    }


    // string name = word.value;
    auto *fncall = new ASTFunctionCall(nullptr);
    // auto elms = grp->elms;
        
    ASTFunctionDefine* fndef(nullptr);

    // 临时用来查询函数的类型
    auto *tmpfty = new TypeFunction(funname);


    // 解析成员函数调用

        // 采用贪婪匹配模式
    while (true) {
        // 匹配函数（第一次无参）
        int match = filter.match(tmpfty);
        if (match==1 && filter.unique) {
            fndef = filter.unique; // 找到唯一匹配
            string idname = fndef->ftype->getIdentify();
            break;
        }
        if (match==0) {
            //prepareBuild(cachebuilds);
            FATAL("No macth function '"+tyclass->name+"."+tmpfty->getIdentify()+"' !");
            // throw ""; // 无匹配
        }
        // 判断函数是否调用结束
        auto word = getWord();
        prepareWord(word); // 上层处理括号
        if (ISWS(End) || ISSIGN(")")) { // 提前手动调用结束
            if (filter.unique) {
                fndef = filter.unique; // 找到唯一匹配
                break;
            }
        }
        // 添加参数
        AST *exp = build();
        if (!exp) {
            if (filter.unique) {
                fndef = filter.unique; // 无更多参数，找到匹配
                break;
            }
            FATAL("No macth function '"+tyclass->name+"."+tmpfty->getIdentify()+"' !");
        }
        //cachebuilds.push_back(exp);
        fncall->addparam(exp); // 加实参
        auto *pty = exp->getType();
        // 重载函数名
        tmpfty->add("", pty);
    }

    fncall->fndef = fndef; // elmfn->fndef;
        

    delete tmpfty;



    // 静态成员函数验证
    //if (is_static && ! fndef->is_static_member) {
    //    FATAL("'"+fndef->ftype->name+"' is not a static member function !")
    //}
    */
    
    auto * mfc = new ASTMemberFunctionCall(val, fncall);

    return mfc;
}

/**
 * 宏解析
 */
AST* Build::buildMacro(ElementLet* let, const string & name)
{
    // 解析参数
    map<string, list<Word>> pmstk;
    for (auto pm : let->params) {
        list<Word> pmws;
        auto word = getWord();
        if(ISSIGN("(")){ // 剥掉外层括号
            cacheWordSegment(pmws);
        } else {
            pmws.push_back(word);
        }
        pmstk[pm] = pmws;
    }

    // 展开宏体
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

    // 预备
    prepareWord(bodys);
    // 调试打印
    DEBUG_WITH("letmacro_words", \
        if(bodys.size()){ \
        cout << "letmacro【"; \
        for (auto &p : bodys) { \
            cout << " " << p.value; \
        } \
        cout << "】" << endl; \
        } \
        )

    // 重新开始解析
    return build();
}

/**
 * 解析子作用域
 */
AST* Build::buildChildScope(const string & name, const string & tip)
{
    // cout << "create new scope !" << endl;
    auto* scope = new ASTChildScope(name);
    auto * old_stack = stack;
    // 名字空间
    string scopename = name;
    // 判断重定义
    auto it = old_stack->spaces.find(scopename);
    if(it!=old_stack->spaces.end()){
        // FATAL("Namespaces '"+scopename+"' cannot redefinition ！")
        stack = it->second; // 分析栈合并
    } else {
        // 新建分析栈
        stack = new Stack(old_stack, Stack::Mod::Namespace);
    }
    if(name==""){
        static int anyidx = 0; // 匿名标记
        anyidx++;
        scopename = "#" + tip + Str::l2s(anyidx);
        stack->mod = Stack::Mod::Anonymous; // 匿名分析栈
    }
    // 添加子栈
    old_stack->spaces[scopename] = stack;

    // 开始验证
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
    // 复位分析栈
    stack = old_stack;
    // 返回子作用域对象
    return scope;
}


/********************************************************/


/**
 * namespace 定义名字空间
 *
AST* Build::build_namespace()
{

    
    Word word = getWord();

    if (NOTWS(Character)) {
        FATAL("namespace define need a legal name to belong !")
    }

    // 历史长度
    int oldlen = defspace.size();

    // 添加命名空间
    defspace += "";// (oldlen == 0 ? "" : "_") + word.value;

    CHECKLPAREN("namespace define need a sign ( to belong !")

    // 循环建立语句
    AST *block = createAST();

    CHECKRPAREN("namespace define need a sign ) to end !")

    // 名字空间复位
    defspace = defspace.substr(0, oldlen);

    return block;
}
*/


/**
 * include 包含并展开文件
 */
AST* Build::build_include()
{
    Error::snapshot();
    AST* path = build(false);
    auto *p = dynamic_cast<ASTConstant*>(path);
    if (!p || !p->type->is(Type::get("String"))) {
        // 不是有效的路径
        Error::exit("Behind 'include' is not a valid file path ! "\
            "(need a type<String> constant value)");
    }

    // 从当前分析的文件定位到下一个文件
    string absfile = Path::join(tkz->file, p->value);
    // cout << "absfile:" << absfile << endl;
    if (!Fs::exist(absfile)) {
        Error::exit("Cannot find the file '"+p->value+"' at current and library path ! ");
    }
    
    Error::backspace(); // 回退

    // include 唯一性
    if (checkSetInclude(absfile)) {
        // 调试打印
        DEBUG_COUT("repeat_include", "[repeat include] "+absfile)

        delete path;
        return new ASTGroup();
    }


    // 挂起缓存的单词
    list<Word> cache = prepare_words;
    prepare_words.clear();

    // 新建并替换词法分析器
    auto *old_tkz = tkz;
    Tokenizer new_tkz(absfile, false);
    tkz = & new_tkz;

    // 执行语法分析
    AST* tree = createAST();

    // 复位词法分析器
    tkz = old_tkz;

    // 复位挂起的单词
    prepare_words = cache;

    // 复位错误报告
    Error::update(old_tkz);

    delete p;
    return tree;
}

/**
 * 初始化变量
 */
AST* Build::build_var()
{    
    Word word = expectIdName("var define need a legal name to belong !");

    string name = word.value;

    // 查找是否存在
    auto fd = stack->find(name, false);
    if (fd) {
        FATAL("Cannot repeat define var '"+name+"' !")
    }
    
    AST* value = build();

    auto vardef = new ASTVariableDefine( name, value );

    // 如果值是变量
    auto *var = new ElementVariable(value->getType());
    if (auto*astv=dynamic_cast<ASTVariable*>(value)) {
        var->origin = astv->origin;
    } else {
        var->origin = value;
    }
    //全局唯一名字
    ASSERT(vardef->unique_name!="", "")
    var->unique_name = vardef->unique_name;
    // 添加变量到栈
    stack->put(name, var);

    return vardef;
}

/**
 * set 变量赋值
 */
AST* Build::build_set()
{
    Word word = getWord();

    if (NOTWS(Character)) {
        FATAL("var assignment need a legal name to belong !")
    }

    string name = word.value;

    // 查找变量是否存在
    auto fd = stack->find(name);
    ElementVariable * ev = dynamic_cast<ElementVariable*>(fd);
    if ( !fd || !ev ) {
        // 变量不存在，查找是否为成员变量
        if (stack->tydef) {
            AST* instance = new ASTVariable( // 类变量
                DEF_MEMFUNC_ISTC_PARAM_NAME,
                stack->tydef->type
            );
            // 赋值类本身
            if (name==DEF_MEMFUNC_ISTC_PARAM_NAME) {
                stack->fndef->is_static_member = false; // 有成员函数
                return instance;
            }
            Type* elmty = validType(stack->tydef->type->elmget(name));
            if (elmty) { // 找到成员
                AST* value = build();
                // 引用类型
                auto * qty = dynamic_cast<TypeRefer*>(elmty);
                // 类型检查
                auto* chty = elmty;
                if (qty) {
                    chty = qty->type;
                }
                if ( ! validType(value->getType())->is(chty)) {
                    FATAL("member assign type not match !")
                }
                stack->fndef->is_static_member = false; // 有成员函数
                // 返回类成员赋值
                Type *elmty = stack->tydef->type->elmget(name);
                // 是否为存引用值
                if (qty) {
                    if (!qty->type) { // 初始化之前不能使用
                        FATAL("Can't use quote value before initialize !")
                    }
                    value = new ASTQuote(value, qty); // 引用载入
                }
                // 返回访问
                return new ASTMemberAssign(
                    instance,
                    stack->tydef->type->elmpos(name),
                    value
                );
            }
        }

        FATAL("var '" + name + "' does not exist can't assignment  !")
    }

    // 类型
    Type* ty = ev->type;

    // 值
    AST* value = build();
    Type* vty = value->getType();

    // 类型检查
    if (!vty->is(ty)) {
        FATAL("can't assignment <"
            +vty->getIdentify()+"> to var "
            +name+"<"+ty->getIdentify()+">  !")
    }

    // 添加变量到栈
    // stack->set(name, new ElementVariable(vty));

    auto * ret = new ASTVariableAssign( name, value );
    // 全局唯一名称
    ret->unique_name = ev->unique_name;
    return ret;

}

/**
 * type 声明类型
 */
AST* Build::build_type()
{
    if (stack->tydef) { // 嵌套定义
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

    // 查询类型是否定义
    if (auto *fd = dynamic_cast<ElementType*>(stack->find(typeName,false))) {
        FATAL("can't repeat type '"+typeName+"' !")
    }

    // 检查括号
    ;
    CHECKLPAREN("namespace define need a sign ( to belong !")

    // 新建类型
    TypeStruct* tyclass = new TypeStruct(typeName);
    if (stack->fndef) {
        tyclass->increment(); // 函数定义名称加上自增索引
    }

    // 生成AST
    ASTTypeDefine* tydef = new ASTTypeDefine();
    tydef->type = tyclass;

    auto *elmty = new ElementType(tyclass);

    // 新栈
    Stack *old_stk = stack;
    Stack *new_stk = new Stack(stack, Stack::Mod::Class);
    // 添加到当前栈帧 支持函数参数
    new_stk->put(typeName, elmty);
    new_stk->tydef = tydef;
    new_stk->fndef = nullptr; // 类成员函数不属于任何包裹函数
    stack = new_stk;
    // 添加子栈
    old_stk->spaces["%" + typeName] = new_stk;


#define TCADD(N) \
    if(it){ \
        tyclass->add(N, it); \
    } else { \
        FATAL("Type declare format error: not find Type <"<<word.value<<"> !") \
    }

    while (1) { // 类型元素定义
        // 成员类型
        Type *it = expectTypeState();
        if (tyclass->is(it)) { // 类成员不能包含自己除非引用
            FATAL("Type declare can't contains it self unless referenced !")
        }
        if(!it) {
            word = getWord();
            if (ISSIGN(")")) {
                break; // 类型声明结束
            }
            if("fun"==word.value){
                build_fun(); continue;
            } else if ("dcl" == word.value) {
                build_dcl(); continue;
            }
            FATAL("Type declare format error: not find Type <" << word.value << "> !")
        }
        // 匿名成员
        while (true) {
            if (Type *ty = expectTypeState()) {
                tyclass->add(it); // 匿名
                it = ty;
            } else break;
        }
        // 成员名称
        word = getWord();
        // 添加成员
        tyclass->add(it, word.value);
    }

#undef TCADD

    // 检查拷贝成员函数列表
    type_member_stack[tyclass] = new_stk;
    type_define[tyclass] = tydef;


    // 添加到分析栈
    stack = old_stk;
    stack->put(typeName, elmty);

    // 返回
    return tydef;
}

/**
 * tydef 类型重命名
 */
AST* Build::build_tydef()
{
    // 类型名称
    Word word = expectIdName("type rename need a legal name to belong !" );
    string typeName = word.value;

    // 解析类型
    Type *ty = expectTypeState();
    if(!ty){
        FATAL("type rename need a legal type to belong !")
    }

    // 添加进分析栈
    auto *elmty = new ElementType(ty);
    stack->put(typeName, elmty);

    // 返回类型重命名
    return new ASTTypeRename(ty, typeName);
}

/**
 * dcl 声明函数
 */
AST* Build::build_dcl()
{
    Word word = expectIdName("function declare need a type name to belong !");

    // 函数返回值类型
    Type *rty;
    Element* elmret = stack->find(word.value);
    if (auto *ety = dynamic_cast<ElementType*>(elmret)) {
        rty = ety->type;
    } else {
        FATAL("function declare need a type name to belong !")
    }

    // 函数名称
    word = getWord();
    if (NOTWS(Character)) {
        FATAL("function declare need a legal name !")
    }

    // 新建函数类型
    string funcname = word.value;
    auto *functy = new TypeFunction(funcname, rty);

    // 括号
    CHECKLPAREN("function declare need a sign ( to belong !")

    // 函数参数解析
    while (true) {
        word = getWord();
        if (ISSIGN(")")) {
            break; // 函数参数列表结束
        }
        Type *pty;  // 参数类型
        if (auto *ety = dynamic_cast<ElementType*>(stack->find(word.value))) {
            functy->add(ety->type); // 匿名
        } else {
            FATAL("function declare parameter format is not valid !")
        }
    }
    
    // 函数是否已经声明
    ASTFunctionDefine* fndef = stack->findFunction(functy);

    // 判断函数是否已经定义
    if (!fndef) {
        fndef = new ASTFunctionDefine(functy, nullptr);
        // 添加新函数
        stack->addFunction(fndef);
    }

    // 返回函数声明
    return new ASTFuntionDeclare(fndef->ftype);
}

/**
 * fun 定义函数
 */
AST* Build::build_fun()
{

    Word word = expectIdName("function define need a type name to belong !");

    // 函数返回值类型
    Type *retty(nullptr); // nullptr 时需要推断类型
    
    // 函数名称
    string funcname = word.value;

    // 是否为类型构造函数
    if(stack->tydef && (
        funcname == tpl_ty_name ||
        funcname == stack->tydef->type->name))
    {
        auto word = getWord();
        if(ISSIGN("(")){
            // 类型中与类型同名且无返回值的为构造函数
            status_construct = true;
            funcname = stack->tydef->type->name;
        } else {
            prepareWord(word);
        }

    }
    
    // 普通函数处理
    if (!status_construct) {
        Element* elmret = stack->find(word.value);
        if (auto *ety = dynamic_cast<ElementType*>(elmret)) {
            retty = ety->type;
        }
        else {
            prepareWord(word);
        }

        // 函数名称
        word = getWord();
        if (NOTWS(Character)) {
            FATAL("function define need a legal name !")
        }
        funcname = word.value; // 函数名

        // 括号验证
        CHECKLPAREN("function define need a sign ( to belong !")
    }

    // 新建函数类型
    // string funcname = word.value;
    // 如果是类型析构函数
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


    // 函数参数解析
    while (true) {
        Type *pty = expectTypeState(); // 参数类型
        if (!pty) {
            word = getWord();
            if (ISSIGN(")")) {
                break; // 函数参数列表结束
            }
            FATAL("Parameter format is not valid !")
        }
        string pnm; // 参数名称
        word = getWord();
        if (NOTWS(Character)) {
            FATAL("function parameter need a legal name !")
        }
        pnm = word.value;
        if (stack->tydef && pnm == DEF_MEMFUNC_ISTC_PARAM_NAME) {
            // 类成员函数，参数名称不能是 this 
            FATAL("can't give a parameter name '" + pnm + "' in type member function !")
        }
        functy->add(pty, pnm);
        // 构造函数入栈
        if (status_construct) {
            constructfuncty->add(pty, pnm);
        }
    }

    // 析构函数 不能有参数
    if(is_delete && functy->types.size()){
        FATAL("Destructors cannot have parameters !")
    }

    // 函数是否已经声明
    ASTFunctionDefine* aldef = stack->findFunction(functy);
    ASTFunctionDefine *fndef;

    // 判断函数是否已经定义
    if (aldef){
        if (aldef->body) {
            // 函数以声明，且 body 已定义
            FATAL("function define repeat: " + functy->str());
        }
        fndef = aldef;
    } else {
        // 新建函数定义
        fndef = new ASTFunctionDefine(functy);
    }

    // 创建新分析栈
    Stack* old_stack = stack;
    auto *new_stack = new Stack(stack, Stack::Mod::Function); // 函数分析栈
    new_stack->fndef = fndef; // 当前定义的函数
    // 提前 添加函数 支持递归
    stack->addFunction(fndef);

    // 添加子栈
    old_stack->spaces["@"+funcname] = new_stack;

    // 函数定义环境
    fndef->wrap = stack->fndef; // wrap
    fndef->belong = stack->tydef; // belong

    // 添加函数实参到分析栈
    int i(0);
    for (auto &pty : functy->types) {
        string pn(functy->tabs[i]);
        //string unpn = ASTVariableDefine::getUniqueName();
        auto ev = new ElementVariable(pty);
        // 设置全局唯一名称，用于内部函数作用域捕获
        //ev->unique_name = unpn;
        new_stack->put(pn, ev); // 加实参
        //new_stack->put(unpn, ev); // 加全局唯一实参
        i++;
    }
    // 替换新栈帧
    stack = new_stack;

    // 新建函数体
    ASTGroup *body;
    word = getWord();
    // 多语句
    if (ISSIGN("(")) {
        body = buildGroup();
        word = getWord();
        if (NOTSIGN(")")) {
            FATAL("Error format function body !")
        }
    // 单语句
    } else {
        body = new ASTGroup();
        body->add(build());
    }
    
    // 加上 Body
    fndef->body = body;
    // 是否为构造函数
    fndef->is_construct = status_construct;

    // 返回值验证与推断，自动添加返回值语句
    retty = _autoAddFuncRet(fndef);

    // 设置可能标记的返回值
    /*
    if (auto*astv=dynamic_cast<ASTVariable*>(cur_fun_ret)) {
        cur_fun_ret = astv->origin;
    }
    // 返回堆内存创建的对象
    if (dynamic_cast<ASTNew*>(cur_fun_ret)) {
        retty = new TypePointer(retty);
    }
    */
    functy->ret = retty;
    fndef->ftype = functy;
    
    // 复位旧栈帧
    stack = old_stack;

    // 构造函数入栈
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

    // 函数已经声明过了，添加 body
    if (aldef) {
        // 替换 ftype，加入参数名称
        // delete aldef->ftype;
        // aldef->ftype = functy;

    // 否则添加新函数
    } else {
        stack->addFunction(fndef);
    }

    // 添加到类成员函数
    if(stack->tydef){
        stack->tydef->members.push_back(fndef);
    }
    
    // 复位
    status_construct = false;

    // 返回值
    return fndef;
}

/**
 * ret 函数值返回
 */
AST* Build::build_ret()
{
    AST *ret = build();
    Type* rty = ret->getType();
    // 验证返回值
    verifyFunctionReturnType( rty );
    // cur_fun_ret = ret;
    return new ASTRet(ret);
}

/**
 * tpf 模板函数推断
 */
AST* Build::build_tpf()
{
    Word word = expectIdName("template function define need a legal name to belong !");
    
    string tpfName = word.value; // fixNamespace(word.value);
    if (stack->find(DEF_PREFIX_TPF + tpfName,false)) {
        FATAL("template function duplicate definition '"+tpfName+"' !");
    }
    
    // 检查括号
    CHECKLPAREN("template function define need a sign ( to belong !")

    // 新建定义
    auto *tpfdef = new ASTTemplateFuntionDefine();
    tpfdef->name = tpfName;

    // 解析形参列表
    while (1) {
        word = getWord();
        if (ISSIGN(")")) { // 形参列表结束
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
    
    // 解析函数体
    cacheWordSegment(tpfdef->bodywords, false); // 缓存括号段内容
    
    // 添加到分析栈
    stack->put(DEF_PREFIX_TPF + tpfName, new ElementTemplateFuntion(tpfdef));

    return tpfdef;
}

/**
 * tpty 类模板
 */
AST* Build::build_tpty()
{
    // 类模板
    auto *tptydef = new ASTTemplateTypeDefine();
    
    // 类模板名称
    Word word = expectIdName("template type define need a legal name to belong !");
    tptydef->name = word.value; // fixNamespace(word.value);
    
    // 检查括号
    CHECKLPAREN("template type define need a sign ( to belong !")

    // 解析模板参数
    while(true){
        word = getWord();
        if(ISSIGN(")")){
            break;
        }
        tptydef->params.push_back(word.value);
    }
    
    // 检查括号
    CHECKLPAREN("template type define need a sign ( to belong !")
    
    // 解析模板实体
    cacheWordSegment(tptydef->bodywords, false);

    // 添加到分析栈
    stack->put(tptydef->name, new ElementTemplateType(tptydef));

    // 返回类模板
    return tptydef;
}

/**
 * if 条件分支
 */
AST* Build::build_if()
{

#define DO_FATAL FATAL("No match bool function Type<"+idn+"> when be used as a if condition")
    
    AST* cond = build();
    Type* cty = cond->getType();
    Type* boolty = Type::get("Bool");

    string idn = cty->getIdentify();

    bool isboolty = cty->is(boolty);
    if ( ! isboolty) { // 非 Bool 类型，查找 bool 函数
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
        // 自动加上 bool 函数调用
        auto fcall = new ASTFunctionCall(ef->fndef);
        fcall->addparam(cond);
        cond = fcall;
    }

    // 新建 if 节点
    ASTIf *astif = new ASTIf(cond);

    // if 语句，子作用域
    astif->pthen = buildChildScope("", "if_then"); // build();

    // 检查 else，子作用域
    auto word = getWord();
    if (ISCHA("else")) {
        astif->pelse = buildChildScope("", "if_else"); // build();
    } else {
        prepareWord(word); // 复位
    }


    // 返回 if 节点
    return astif;
}

/**
 * while 循环结构
 */
AST* Build::build_while()
{
#define DO_FATAL FATAL("No match bool function Type<"+idn+"> when be used as a while flow !")
    
    AST* cond = build();
    Type* cty = cond->getType();
    Type* boolty = Type::get("Bool");

    string idn = cty->getIdentify();

    bool isboolty = cty->is(boolty);
    if ( ! isboolty) { // 非 Bool 类型，查找 bool 函数
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
        // 自动加上 bool 函数调用
        auto fcall = new ASTFunctionCall(ef->fndef);
        fcall->addparam(cond);
        cond = fcall;
    }

    // 新建 while 节点
    ASTWhile *astwhile = new ASTWhile(cond);

    // while 语句，子作用域
    astwhile->body = buildChildScope("", "while"); //build();

    // 返回 if 节点 
    return astwhile;

}

/**
 * let 符号绑定
 */
AST* Build::build_let()
{
    auto *let = new ElementLet();
    auto *relet = new ASTLet();

    auto word = getWord();
    
    // 唯一名称
    string idname("");
    bool sign = false;

    // 参数宏绑定
    if (ISWS(Character)) {
        idname = word.value;
        relet->head.push_back(idname);
        // 检查括号
        Word word;
        CHECKLPAREN("let macro binding need a sign ( to belong !")
        // 参数
        while (true) {
            word = getWord();
            if (ISSIGN(")")) {
                break; // 参数结束
            }
            string str(word.value);
            let->params.push_back(str);
            relet->head.push_back(str);
        }

    // 操作符绑定
    } else if (ISSIGN("(")) {
        
        // 参数
        while (true) {
            word = getWord();
            if (ISSIGN(")")) {
                break; // 参数结束
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
        ) { // 至少提供一个符号 且不能为前缀符号
            FATAL("let operator binding format error !")
        }

    } 
   
    auto *fd = stack->find(idname, false);
    if (fd) { // 不能重复绑定
        FATAL("let can't repeat binding '"+idname+"' !")
    }
    
    // 检查括号
    CHECKLPAREN("let binding need a sign ( to belong !")

    // 绑定体
    list<Word> bodys;
    cacheWordSegment(bodys);
    for (auto &i : bodys) {
        let->bodywords.push_back(i);
        relet->body.push_back(i.value);
    }

    // 添加栈到绑定
    stack->put(idname, let);

    // 返回
    return relet;
}

/**
 * member call 成员函数调用
 */
AST* Build::build_elmivk()
{
    TypeStruct* tyclass(nullptr);
    AST* val(nullptr);

    auto word = getWord();

     // 通过类型名称调用静态成员函数
    bool is_static = false;
    if (auto *e = dynamic_cast<ElementType*>(
        stack->find(word.value))) {
        if (tyclass = dynamic_cast<TypeStruct*>(e->type)) {
            is_static = true;
        }
    }
    // 通过类名称调用静态函数
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

    // 类内部栈
    auto target_stack = type_member_stack[tyclass];

    // 成员函数名称
    word = getWord();
    string fname = word.value;

    // 函数调用
    ASTFunctionCall* fncall =
        _functionCall(fname ,target_stack, false);

    if ( ! fncall) {
        FATAL("can't find member function '"
            +fname+"' in class '"+tyclass->name+"'")
    }


    // 静态成员函数验证
    if (is_static && ! fncall->fndef->is_static_member) {
        FATAL("'"+fncall->fndef->ftype->name+"' is not a static member function !")
    }

    auto * mfc = new ASTMemberFunctionCall(val, fncall);

    return mfc;

}

/**
 * member get 成员访问
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

    // 索引检查
    if (pos<0 || pos>scty->len()) {
        FATAL("class '"+scty->name+"' no element '" + word.value + "' !")
    }
    ins->index = pos;

    // 是否为取引用值
    if (auto * qty = dynamic_cast<TypeRefer*>(scty->types[pos])) {
        if (!qty->type) { // 初始化之前不能使用
            FATAL("Can't use quote value "+scty->name+"."+word.value+" before initialize !")
        }
        return new ASTLoad(ins, qty); // 引用载入
    }

    return ins;
}

/**
 * member set 成员赋值
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

    // 索引检查
    if (pos<0 || pos>scty->len()) {
        FATAL("class '"+scty->name+"' no element '" + word.value + "' !")
    }
    ins->index = pos;
    
    AST* putv = build();
    Type* putty = putv->getType();
    // 类型检查
    Type* pvty = scty->types[ins->index];
    if (auto * qty = dynamic_cast<TypeRefer*>(pvty)) {
        // 引用类型
        if (qty->type && !putty->is(qty->type)) {
            FATAL("can't quote member assign <"+pvty->getIdentify()+"> by <"+putty->getIdentify()+">' !")
        }
        // 第一次赋值固定类型
        if (!qty->type) {
            qty->type = putty;
        }
        // 取得引用
        putv = new ASTQuote(putv, qty);

    } else if (!putty->is(pvty)) {
        FATAL("can't member assign <"+pvty->getIdentify()+"> by <"+putty->getIdentify()+">' !")
    }

    ins->value = putv;

    return ins;
}

/**
 * member function 外部定义
 */
AST* Build::build_elmdef()
{
    auto word = getWord();
    // 函数返回值类型
    TypeStruct *ty(nullptr);
    if(auto *e=dynamic_cast<ElementType*>(stack->find(word.value))){
        if (auto *y = dynamic_cast<TypeStruct*>(e->type)) {
            ty = y;
        }
    } 
    // 必须为类型
    if (NOTWS(Character) || ! ty) {
        FATAL("member function external define need a type name to belong !")
    }

    // 替换分析栈
    Stack* old_stack = stack;
    stack = type_member_stack[ty];
    stack->tydef = type_define[ty];

    // 定义
    AST* res = build();

    // 复位栈帧
    stack = old_stack;

    return new ASTExternalMemberFunctionDefine(ty, res);
}

/**
 * Multiple macro _ 占位符 重复宏
 */
AST* Build::build_mcrfor()
{
    // 检查括号
    Word word;
    CHECKLPAREN("multiple macro need a sign ( to belong !")

    // 重复宏参数
    list<list<Word>> argvs;
    while (true) {
        list<Word> item;
        word = getWord();
        if (ISSIGN(")")) {
            break; // 参数结束
        } else if (ISSIGN("(")) {
            cacheWordSegment(item);
        } else {
            item.push_back(word);
        }
        argvs.push_back(item);
    }
    
    // 检查括号
    CHECKLPAREN("multiple macro need a sign ( to belong !")

    // 重复宏体
    list<Word> bodys;
    while (true) {
        word = getWord();
        if (ISSIGN(")")) {
            break; // 参数结束
        }
        bodys.push_back(word);
    }
    
    // 解析生成
    list<Word> prepares;
    size_t i(0);
    for (auto & a : argvs) {
        for (auto & word : bodys) {
            if (ISCHA("_v")) { // 值
                for(auto &w : a){
                    prepares.push_back(w);
                }
            } else if (ISCHA("_i")){ // 索引
                prepares.push_back(Word(Tokenizer::State::Number,Str::l2s(i)));
            } else {
                prepares.push_back(word);
            }
        }
        i++;
    }

    // 预备
    prepareWord(prepares);
    
    // 调试打印
    DEBUG_WITH("mulmcr_words", \
        if(prepares.size()){ \
        cout << "mulmcr【"; \
        for (auto &p : prepares) { \
            cout << " " << p.value; \
        } \
        cout << "】" << endl; \
        } \
        )


    // 重新生成
    return build();

}

/**
 *  macro if 条件宏展开
 */
AST* Build::build_mcrif()
{
    Word word;
    auto word1 = getWord();
    auto word2 = getWord();
    
    // 检查括号
    CHECKLPAREN("condition macro need a sign ( to belong !")

    // 宏体
    list<Word> bodys;
    cacheWordSegment(bodys);

    // 判断条件是否一致
    if (word1==word2) {
        prepareWord(bodys);
    }

    // 重新解析
    return build();

}

/**
 *  macro cut 宏分段
 * 【暂不可用！！！】
 */
AST* Build::build_mcrcut()
{
    Word word = getWord();

    // cout << "：：：：：" << word.value << endl;
    
    // 必须为数字
    if (NOTWS(Number)) {
        FATAL("macro cut need a <Number> belong !")
    }

    size_t seg = Str::s2l(word.value);

    // 括号
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
 *  macro link 宏连接两个词
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
 * array 新建数组类型对象
 */
AST* Build::build_refer()
{
    // 只能在类成员或数组中使用引用类型
    FATAL("only in class or array can use reference type !")
}

/**
 * array 新建数组类型对象
 */
AST* Build::build_array()
{
    Word word = getWord();
    // 数组大小 必须为数字
    if (NOTWS(Number)) {
        FATAL("array define need a <Number> belong !")
    }
    size_t len = Str::s2l(word.value);
    
    // 元素类型
    word = getWord();

    if (NOTWS(Character)) {
        FATAL("array define need a legal type name to belong !")
    }
    
    // 如果为引用类型
    bool is_ref = false;
    if(ISCHA(DEF_TYPE_KEYWORW_REFERENCE)) {
        is_ref = true;
        word = getWord();
    }

    // 类型
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

    // 数组类型

    auto *aryty = new TypeArray(ty, len);

    // 返回数组构造
    return new ASTArrayConstruct(aryty);

}

/**
 * tuple 新建元组类型对象
 */
AST* Build::build_tuple()
{
    Word word; //
    
    // 检查括号
    CHECKLPAREN("tuple define need a sign ( to belong !")
        
    // 新建元组类型（匿名）
    auto *tupty = new TypeStruct("");
    auto *tupval = new ASTTypeConstruct(tupty);

    // 元组元素
    while(true){
        AST* li = build();
        if (nullptr==li) {
            break;
        }
        tupval->add(li);
        tupty->add(li->getType());
    }
    
    // 检查括号
    CHECKRPAREN("tuple define need a sign ) to belong !")
        
    // 返回元组值
    return tupval;

}

/**
 * array 数组成员访问
 */
AST* Build::build_arrget()
{
    // 数组对象
    AST *ary = build();

    // 类型检测
    TypeArray * arty(nullptr);
    if ( ! (arty=validTypeArray(ary))) {
        FATAL("array visit must use to array type !")
    }
    
    // 索引
    AST *idx = build();

    // 类型检测
    if (!dynamic_cast<TypeInt*>(idx->getType())) {
        FATAL("array visit must get a Number type index !")
    }
    
    // 数组元素访问
    AST* visit = new ASTArrayVisit(ary, idx);

    // 如果是引用类型
    if (auto *refty = dynamic_cast<TypeRefer*>(arty->type)) {
        // 载入数据
        visit = new ASTLoad(visit, refty);
    }

    return visit;
}

/**
 * array 数组成员赋值
 */
AST* Build::build_arrset()
{
    // 数组对象
    AST *ary = build();

    // 类型检测
    TypeArray * arty(nullptr);
    if (! (arty=validTypeArray(ary))) {
        FATAL("array assign must use to array type !")
    }
    
    // 索引
    AST *idx = build();

    // 类型检测
    if (!dynamic_cast<TypeInt*>(idx->getType())) {
        FATAL("array assign must get a Number type index !")
    }
    
    // 值
    AST *value = build();
    Type* vty = value->getType();


    // 如果是引用类型
    if (auto *refty = dynamic_cast<TypeRefer*>(arty->type)) {
        // 取得引用地址值
        Type* tarty = refty->type;
        if (! vty->is(tarty)) {
            FATAL("can't quote member assign <"
                +tarty->getIdentify()+"> by <"+vty->getIdentify()+">' !")
        }
        value = new ASTQuote(value, refty);
        
    // 类型检测
    } else if (! vty->is(arty->type)) {
        FATAL("can't quote member assign <"
            +arty->getIdentify()+"> by <"+vty->getIdentify()+">' !")
    }
    
    // 返回数组元素赋值
    return new ASTArrayAssign(ary, idx, value);
}

/**
 * adt 适配器模式
 */
AST* Build::build_adt()
{
    setModADT(true);
    return build();
}

/**
 * new 堆内存申请
 */
AST* Build::build_new()
{
    Word word = getWord();
    
    AST* new_len(nullptr);
    Type* intty = Type::get("Int");

    // 如果是申请数组
    if (ISCHA(DEF_TYPE_KEYWORW_ARRAY)) {
        new_len = build();
        if ( ! new_len->getType()->is(intty)) {
            FATAL("new array length must be a Int type !")
        }
        Type* scty = expectTypeState();
        // 申请内存
        auto *vptr = new ASTMalloc(scty, new_len);
        vptr->is_array = true;
        // 返回
        return new ASTNew(vptr);
    }
     
    // 普通对象空间 
    new_len = new ASTConstant(intty, "1");
   
    // 类型
    // word = getWord();
    ElementType* ety(nullptr);
    if (NOTWS(Character) || !(ety=dynamic_cast<ElementType*>(stack->find(word.value, true)))) {
        FATAL("new format error !" + Str::l2s((int)ety))
    }
    
    TypeStruct* scty(nullptr);
    if (! (scty=dynamic_cast<TypeStruct*>(ety->type))) {
        FATAL("new object type error !")
    }
    // 申请内存
    AST * vptr = new ASTMalloc(scty, new_len);

    // 调用构造函数
    AST * cons = buildConstruct(scty, vptr);

    // 返回
    return new ASTNew(cons);
}

/**
 * delete 堆内存释放
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
 * copy 拷贝对象
 */
AST* Build::build_copy()
{
    AST *obj = build();
    
    // 类型检测
    if (!dynamic_cast<TypeStruct*>(obj->getType())) {
        FATAL("copy must use to a Class type !")
    }

    // 拷贝
    return new ASTCopy(obj);
}

/**
 * 全局唯一名称 定义
 */
AST* Build::build_uvnnew()
{
    auto word = getWord();
    string key = word.value;
    
    // 检查是否重定义
    auto it = uq_var_names.find(key);
    if(it != uq_var_names.end()){
        FATAL("Repeated initializat unique variable name : "+key)
    }

    // 自增量
    uq_var_autoincrement++;
    string uvn =  "%" + key + Str::l2s(uq_var_autoincrement);
    uq_var_names[key] = uvn;

    // 返回 AST
    return new ASTUVNnew(key, uvn);
}

/**
 * 全局唯一名称 获取
 */
AST* Build::build_uvnget()
{
    // 回去唯一名称
    Word wd = getUniqueVariableName(getWord().value);

    // 缓存
    prepareWord(wd);

    // 重新开始
    return build();
}

/**
 * 全局唯一名称 删除
 */
AST* Build::build_uvndel()
{
    auto word = getWord();
    string key = word.value;
    
    // 检查是否重定义
    auto it = uq_var_names.find(key);
    if(it == uq_var_names.end()){
        FATAL("Cannot to delete not exist unique variable name : "+key)
    }

    // 删除
    uq_var_names.erase(it);

    return new ASTUVNdel(key);
}

/**
 * 全局唯一名称 清空
 */
AST* Build::build_uvnclear()
{
    uq_var_names.clear();

    return new ASTUVNclear();
}

/**
 * scope 局部作用域，名字空间声明
 */
AST* Build::build_scope()
{
    Word word = getWord();
    string space = "";

    // 名字空间
    if(ISWS(Character)){
        space = word.value;
    } else {
        prepareWord(word);
    }

    // 建立
    return buildChildScope(space);
}

/**
 * uscp 加载名字空间
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

    // 返回
    return new ASTUseScope(name+")");
}

/**
 * build_delscp 卸载名字空间
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
    // 返回
    return new ASTDeleteScope(name+")");
}


/**
 * link 得到变量的引用
 *
AST* Build::build_link()
{
    AST* value = build();
    return new ASTQuote(value);
}

/**
 * load 从引用得到变量
 *
AST* Build::build_load()
{
    AST* value = build();
    return new ASTLoad(value);
}

*/



/********************************************************/


/**
 * 预测是否需要解开符号绑定
 */
bool Build::forecastOperatorBind()
{
    bool spread = false; // 是否延展
    bool fail = false;
    list<Word> cache;
    while (true) {
        auto word = getWord();
        if (ISWS(End)) { // 结束
            break; // 结束
        }
        cache.push_back(word);
        if (ISSIGN("(")) { // 子级
            if (fail) break;
            cacheWordSegment(cache, false); // 缓存括号段内容
            fail = true;
            continue;
        }
        if (ISWS(Operator)) {
            spread = true; // 需要解绑
            break;
        }
        if (ISWS(Sign)) { // 其它符号
            break;
        }
        if (fail) {
            break; // 否
        }
        fail = true;
    }
    
    /*
    cout << "cache【";
    for (auto &p : cache) {
        cout << " " << p.value;
    }
    cout << "】" << endl;
    */

    // 恢复
    prepareWord(cache);

    return spread;
}


/**
 * let 操作符绑定绑定展开
 */
AST* Build::buildOperatorBind()
{
    //无展开返回
    if (! forecastOperatorBind()) {
        return nullptr; 
    }

    // 展开绑定
    auto words = spreadOperatorBind();
    prepareWord(words); // 展开延展后的内容

    // 不再次检测是否延展
    return build(false);
}


/**
 * let 操作符绑定绑定展开
 */
list<Word> Build::spreadOperatorBind(list<Word>*pwds)
{
    string idname("");
    vector<list<Word>> params;

    if (pwds) { // 左侧参数
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
        if (ISWS(End)) { // 文本结束
            end = true;
        }
        cache.push_back(word);
        if (ISSIGN("(")) { // 子级
            cacheWordSegment(cache, false); // 缓存括号段内容
            if (cache.empty()) {
                FATAL("Operator binding priority error !")
            }
            params.push_back(cache);
            idname += DEF_SPLITFIX_OPRT_BIND;
        }else if (ISWS(Operator)) { // 操作符
            idname += word.value;
        } else {
            idname += DEF_SPLITFIX_OPRT_BIND;
            list<Word> pms;
            pms.push_back(word);
            params.push_back(pms);
        }
        if ( ! filter) { // 初始化筛选器
            filter = new filterLet(stack, idname);
        }
        // 筛选匹配
        int match = filter->match(idname);
        if (match==1 && filter->unique) { // 贪婪匹配完成
            let = filter->unique;
            break; // 找到唯一匹配
        }
        if(end||match==0){
            if (let) {
                prepareWord(cache); // 复位多余内容
                break; // 返回上一步的全匹配
            }
            FATAL("can't find the binding '"+idname+"' !")
        }
        // 记录上一步的唯一匹配
        let = filter->unique;
    }

    delete filter;

    // 解析参数
    map<string, list<Word>> pmstk;
    int i = 0;
    for (auto &p : let->params) {
        pmstk[p] = params[i];
        i++;
    }

    // 展开绑定
    list<Word> results;
    for (auto &word : let->bodywords) {
        if (ISWS(Character) && pmstk.find(word.value) != pmstk.end()) {
            // 解开参数
            auto wd = pmstk[word.value];
            for(auto &w : wd){
                results.push_back(w);
            }
        }else {
            results.push_back(word);
        }
    } 

    // 调试打印
    DEBUG_WITH("binding_spread", \
        if(results.size()){ \
        cout << "binding spread "+idname+"【"; \
        for (auto &p : results) { \
            cout << " " << p.value; \
        } \
        cout << "】" << endl; \
        } \
    )

    // 判断下一个是否为符号绑定
    auto word = getWord();
    prepareWord(word);
    if (ISWS(Operator)){
        return spreadOperatorBind(&results);/*
        filterLet filter(stack, DEF_SPLITFIX_OPRT_BIND + word.value);
        if (filter.size()>0) { // 查询匹配
            return spreadOperatorBind(&results);
        }*/
    }


    // 符号绑定结束
    return results;
}


/********************************************************/


/**
 * 缓存单词段（包含括号内部所有内容）
 */
void Build::cacheWordSegment(list<Word>& cache, bool strip)
{
    int down = 1;
    while (true) {
        auto word = getWord();
        if (ISWS(End)) { // 结束
            return;
        }
        cache.push_back(word);
        if (ISSIGN("(")) down++;
        if (ISSIGN(")")) down--;
        if (0 == down) break;
    }
    if (strip) {
        cache.pop_back(); // 去掉尾部括号
    }
}
void Build::cacheWordCell(list<Tokenizer::Word>& wds, bool strip)
{
    Word word = getWord();
    if(ISSIGN("(")){
        if(!strip) wds.push_back(word);
        cacheWordSegment(wds, strip); // 需要括号
    } else {
        wds.push_back(word);
    }
}

/**
 * 获得类型标注
 */
def::core::Type* Build::expectTypeState()
{
    // 数组或引用类型标记
    string mark = "";
    std::stack<int> array_sizes;

    list<Word> caches;

    Word word;

#define READWORD word = getWord(); caches.push_back(word);
#define RETURNNULL prepareWord(caches); return nullptr;

    // 检测类型标记
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
            if (ISWS(Number)) { // 是否给出数组大小
                array_sizes.push(Str::s2l(word.value));
            } else {
                array_sizes.push(0);
                prepareWord(word);
            }
        } else {
            break;
        }
    }

    // 查找类型
    Type *ty(nullptr);
    Element* res = stack->find(word.value);
    // 普通类型
    if (auto *dco = dynamic_cast<ElementType*>(res)) {
        ty = dco->type;
    // 模板类型
    } else if (auto *ttd = dynamic_cast<ElementTemplateType*>(res)) {
        ty = _templateType(ttd->tptydef);
    } else {
        RETURNNULL
    }

    // 组合类型
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
 * 获得有效的标示符名称
 */
Word Build::expectIdName(const string & error)
{
    Word word = getWord();
    
    if (NOTWS(Character)) {
        FATAL(error)
    }

    // 宏连接
    if (word.value == "mcrlnk") {
        Word word1 = getWord();
        Word word2 = getWord();
        word1.value += word2.value;
        return word1;
    }

    // 全局唯一名
    if(word.value == "uvnget"){
        Word word1 = getWord();
        return getUniqueVariableName(word1.value);
    }

    return word;
}

/**
 * 全局唯一名称获取
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
 * 建立函数调用
 */
TypeFunction* Build::_functionType(bool declare)
{
    return nullptr;
}

/**
 * 建立函数调用
 */
ASTFunctionCall* Build::_functionCall(const string & fname, Stack* stack, bool up)
{
    auto *fncall = new ASTFunctionCall(nullptr);
    ASTFunctionDefine *fndef(nullptr);

    // 节点缓存
    list<AST*> cachebuilds;

    // 临时用来查询函数的类型
    auto *tmpfty = new TypeFunction(fname);
    filterFunction filter = filterFunction(stack, fname, up);
    if (!filter.size()) {
        return nullptr; // 无函数
    }

    // 采用贪婪匹配模式
    while (true) {
        // 匹配函数（第一次无参）
        int match = filter.match(tmpfty);
        if (match==1 && filter.unique) {
            fndef = filter.unique; // 找到唯一匹配
            string idname = fndef->ftype->getIdentify();
            break;
        }
        // 无匹配
        if (match==0) {
            prepareBuild(cachebuilds); // 复位建立的节点
            delete tmpfty;
            delete fncall;
            return nullptr;
        }
        // 判断函数是否调用结束
        auto word = getWord();
        prepareWord(word); // 上层处理括号
        if (ISWS(End) || ISSIGN(")")) { // 提前手动调用结束
            if (filter.unique) {
                fndef = filter.unique; // 找到唯一匹配
                break;
            }
        }
        // 添加参数
        AST *exp = build();
        if (!exp) {
            if (filter.unique) {
                fndef = filter.unique; // 无更多参数，找到匹配
                break;
            }
        }
        cachebuilds.push_back(exp);
        fncall->addparam(exp); // 加实参
        auto *pty = validType(exp); // 合法的参数类型
        // 重载函数名
        tmpfty->add(pty); // 匿名
    }

    fncall->fndef = fndef; // elmfn->fndef;
       
    // 函数调用解析成功
    delete tmpfty;
    return fncall;
}

/**
 * 自动添加返回值
 */
def::core::Type* Build::_autoAddFuncRet(ASTFunctionDefine* fndef)
{

    // 检查返回值类型一致性
#define RETUTN verifyFunctionReturnType(tyret); return tyret;

    Type* tyret = Type::get("Nil");
    // 构造函数无返回值
    if (fndef->is_construct) {
        RETUTN
    }
    // 非空
    size_t len = fndef->body->childs.size();
    while (len--) {
        AST *last = fndef->body->childs[len];
        if ( !last->isCodegen()) {
            continue; // 
        }
        if (last->isValue()) {
            tyret = last->getType();
            // 自动加上 ret 语句
            if ( ! dynamic_cast<ASTRet*>(last)) {
                // cur_fun_ret = last; // 返回节点
                fndef->body->childs[len] = new ASTRet(last);
            }
            RETUTN
        }
        break;
    }

    // 添加 void 返回值
    fndef->body->add(new ASTRet(nullptr));
    // 检查返回值类型一致性
    RETUTN
#undef RETUTN
}

/**
 * 解析模板得到新的类型
 */
TypeStruct* Build::_templateType(ASTTemplateTypeDefine* tptydef)
{
    // 新类型
    // TypeStruct* typety = new TypeStruct(name);
    
    // 创建新类型
    // auto *tydef = new ASTTypeDefine();

    // auto * tptydef = tpty->tptydef;
    
    // 类型名称生成
    string tptyid("");
    string tptyname("");

    // 获取类模板实参
    // size_t arglen = tptydef->params.size();
    // 参数
    map<string, Word> args;
    for(auto k : tptydef->params){
        auto word = getWord();
        // 检查是否为类型
        if(auto*elm=dynamic_cast<ElementType*>(stack->find(word.value))){
            args[k] = word;
            tptyname += "," + elm->type->getIdentify(); // 名称
            tptyid += "," + Str::l2s((int)(elm->type)); // 内存id
        } else{
            FATAL("Type template call need a valid type name !")
        }
    }
    tptyid[0] = '<';
    tptyname[0] = '<';
    tptyid = Str::l2s((int)(tptydef)) + tptyid + ">";
    tptyname = tptydef->name + tptyname + ">";

    // 检查类模板分析栈，是否有生成的类型
    if(auto *elmty = dynamic_cast<ElementType*>(tpty_stk->find(tptyid))){
        // 无需重新生成
        // cout << "_templateType cache: " << tptyid << endl;
        return (TypeStruct*)elmty->type;
    }
    

    // 解析类模板正文，实参替换
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
        // 替换
        body.push_back(fd->second);
    }

    // 预备
    prepareWord(body);
    

    // 设定模板状态
    tpl_ty_name = tptydef->name;
    // 创建新类型至类模板栈
    auto ty = ((ASTTypeDefine*)build_type())->type;
    // 清除状态
    tpl_ty_name = "";
    
    // 添加到类模板栈做缓存 
    tpty_stk->put(tptyid, new ElementType(ty));

    // 返回类型
    return ty;
}


/**
 * 加载与卸载名字空间
 */
bool Build::_useScope(const string & n)
{
    string name = n;
    // 开始查找分析栈
    Stack * stk = stack->use(name);
    if(!stk){
        FATAL("Cannot find the namespace '"+name+"' !")
    }
    // 添加到使用的分析栈
    // 最后添加的栈，最新使用
    stack->uscps.push_front(make_tuple(name, stk));
    return true;
}
bool Build::_delScope(const string & n)
{
    string name = n;
    // 查找
    for(auto one : stack->uscps){
        if(std::get<0>(one)==name){
            // 查找成功，卸载
            stack->uscps.remove(one);
            return true;
        }
    }
    FATAL("Cannot Cancer the not existence namespace '"+name+"' !")
}






