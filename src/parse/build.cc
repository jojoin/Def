/**
 * 语法分析空间
 */


#include "build.h"
#include "../core/error.h"

#include "../util/path.h"
#include "../util/fs.h"
#include "../sys/debug.h"

#include "../core/splitfix.h"
#include "../core/ast.h"
#include "../core/builtin.h"

using namespace std;
using namespace def::core;
using namespace def::util;
using namespace def::sys;
using namespace def::compile;


namespace def {
namespace parse {

    
#define Word Tokenizer::Word 
#define State Tokenizer::State
    
#define ISWS(TS) word.state==State::TS
#define NOTWS(TS) !(ISWS(TS))
#define ISSIGN(S) ISWS(Sign)&&word.value==S
#define NOTSIGN(S) !(ISSIGN(S))
#define ISCHA(S) ISWS(Character)&&word.value==S

    
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
        
    // block
    } else if(ISSIGN("(")){
        ASTGroup* grp = buildGroup();
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
        
        // 类型构造 ？
        if (auto gr = dynamic_cast<ElementType*>(res)) {

            auto *claty = (TypeStruct*)gr->type;
            auto *cst = new ASTTypeConstruct(claty);
            int plen = claty->len();
            while (plen--) { // 添加类型构造参数
                cst->add(build());
            }
            return cst;
        }

        // 函数调用 ？
        if (auto gr = dynamic_cast<ElementGroup*>(res)) {
            try {
                return buildFunctionCall(chara, gr);
            } catch (...) {
                // do nothing
                // cout << chara+": no function match" << endl;
            }
        }

        // 模板函数调用？
        res = stack->find(DEF_SPLITFIX_TPFPREFIX+chara, true);
        if (res) {
            return buildTemplateFuntion(chara, (ElementTemplateFuntion*)res);
        }

        // 语言核心定义？
        AST *core = buildCoreDefine(chara);
        if (core) {
            return core;
        }

        // 语言核心指令？
        /*
        AST *ins = buildInstruction(chara);
        if (ins) {
            return ins;
        }
        */

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

/**
 * 变量或类成员访问
 */
AST* Build::buildVaribale(Element* elm, const string & name)
{
    if (auto gr = dynamic_cast<ElementVariable*>(elm)) {
        return new ASTVariable( name, gr->type );
    }

    // 是否为类成员访问
    if(stack->tydef && stack->fundef){
        AST* instance = new ASTVariable( // 类变量
            DEF_MEMFUNC_ISTC_PARAM_NAME,
            stack->tydef->type
        );
        // 访问类本身
        if (name==DEF_MEMFUNC_ISTC_PARAM_NAME) {
            stack->fundef->is_static_member = false; // 有成员函数
            return instance;
        }
        int i = 0;
        for (auto &p : stack->tydef->type->tabs) {
            if (p==name) { // 找到
                // 返回类成员访问
                AST* elmget = new ASTMemberVisit(instance, i);
                stack->fundef->is_static_member = false; // 有成员函数
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
 * 函数调用
 */
AST* Build::buildFunctionCall(const string & name, ElementGroup* grp, bool istpf)
{
    auto *fncall = new ASTFunctionCall(nullptr);
    auto elms = grp->elms;

    // 节点缓存
    list<AST*> cachebuilds;
        
    // string fname(name); // 函数参数组装名
    // vector<Element*> matchs; // 匹配的函数
    // ElementFunction* elmfn;
    ASTFunctionDefine* fundef(nullptr);

    // 临时用来查询函数的类型
    auto *tmpfty = new TypeFunction(name);
    filterFunction filter = filterFunction(stack, name);

    // 采用贪婪匹配模式
    while (true) {
        // 匹配函数（第一次无参）
        int match = filter.match(tmpfty);
        if (match==1 && filter.unique) {
            fundef = filter.unique; // 找到唯一匹配
            string idname = fundef->ftype->getIdentify();
            break;
        }
        if (match==0) {
            prepareBuild(cachebuilds);
            throw "No macth function !"; // 无匹配
        }
        // 判断函数是否调用结束
        auto word = getWord();
        prepareWord(word); // 上层处理括号
        if (ISWS(End) || ISSIGN(")")) { // 提前手动调用结束
            if (filter.unique) {
                fundef = filter.unique; // 找到唯一匹配
                break;
            }
        }
        // 添加参数
        AST *exp = build();
        if (!exp) {
            if (filter.unique) {
                fundef = filter.unique; // 无更多参数，找到匹配
                break;
            }
        }
        cachebuilds.push_back(exp);
        fncall->addparam(exp); // 加实参
        auto *pty = getType(exp);
        // 重载函数名
        tmpfty->add("", pty);
    }

    fncall->fndef = fundef; // elmfn->fndef;
        
    return fncall;

}

/**W
 * 解析模板函数
 */
AST* Build::buildTemplateFuntion(const string & name, ElementTemplateFuntion* tpf)
{
    // 新函数类型
    TypeFunction* functy = new TypeFunction(name);
    
    // 创建新函数
    auto *fundef = new ASTFunctionDefine(functy);

    // 创建模板函数调用
    auto *fcall = new ASTFunctionCall(fundef);

    // 创建新分析栈
    Stack* old_stack = stack;
    Stack new_stack(stack);
    fundef->wrap = stack->fundef; // wrap
    new_stack.fundef = fundef; // 当前定义的函数

    // ElementStack nsk; // 缓存旧变量
    // 实参入栈


    //list<AST*> paramcaches;
    // ElementStack nsk; // 缓存旧变量
    for (auto &pn : tpf->tpfdef->params) {
        AST* p = build(); // 不取缓存
        //paramcaches.push_back(p); // 缓存
        fcall->addparam(p);
        Type *ty = getType(p);
        functy->add(pn, ty); // 参数类型
        new_stack.put(pn, new ElementVariable(ty)); // 加实参
        //nsk[pn] = old; // 缓存被覆盖的
    }

    /*
    int i(0);
    for (auto &pty : functy->types) {
        string pn(functy->tabs[i]);
        new_stack.put(pn, new ElementVariable(pty)); // 加实参
        i++;
    }
    */

    // 替换新栈帧
    stack = & new_stack;
    
    // 函数体词组
    // prepare_words = tpf->tpfdef->bodywords;
    prepareWord(tpf->tpfdef->bodywords);

    // 解析函数体
    ASTGroup *body = createAST();
    auto word = getWord();
    if (NOTSIGN(")")) {
        FATAL("Error format function body !")
    }

    // 创建函数体
    // 添加新函数
    Type* tyret(nullptr);
    size_t bodylen = body->childs.size();
    if(bodylen>0){
        // 获取函数体最后一句为返回类型
        tyret = getType(body->childs[bodylen-1]);
    }

    // 检查返回值类型一致性
    verifyFunctionReturnType(tyret);

    if ( ! functy->ret) {
        cout << "! functy->ret" << endl;
        functy->ret = Type::get("Nil");
    }

    // 复位旧栈帧
    stack = old_stack;
    // 打印语法分析栈
    DEBUG_WITH("als_stack", \
        cout << endl << endl << "==== Analysis stack ( template function "+name+" ) ===" << endl << endl; \
        new_stack.print(); \
        cout << endl << "====== end ======" << endl << endl; \
        )


    // 设置可能标记的返回值
    functy->ret = tyret;
    fundef->ftype = functy;
    // 加上 Body
    fundef->body = body;

    
    // 复位旧栈帧
    stack = old_stack;

    // 添加新函数
    stack->addFunction(fundef);

    // 设置缓存的参数
    // prepareBuild(paramcaches);

    
    return fcall;


    // 解析函数
    /*
    if (auto res = dynamic_cast<ElementGroup*>(stack->find(name, true))) {
        return buildFunctionCall(name, res, true);
    } else {
        FATAL("Add new function fail !")
    }
    */

    // TODO:: 创建函数调用
}

/**
 * namespace 定义名字空间
 */
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

    word = getWord();
    if (NOTSIGN("(")) {
        FATAL("namespace define need a sign ( to belong !")
    }

    // 循环建立语句
    AST *block = createAST();

    word = getWord();
    if (NOTSIGN(")")) {
        FATAL("namespace define need a sign ) to end !")
    }

    // 名字空间复位
    defspace = defspace.substr(0, oldlen);

    return block;
}

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
    Word word = getWord();

    if (NOTWS(Character)) {
        FATAL("var define need a legal name to belong !")
    }

    string name = word.value;

    // 查找是否存在
    auto fd = stack->find(name);
    if (fd) {
        FATAL("Cannot repeat define var '"+name+"' !")
    }
    
    AST* value = build();

    auto vardef = new ASTVariableDefine( name, value );

    // 添加变量到栈
    stack->put(name, new ElementVariable(getType(value)));

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
                stack->fundef->is_static_member = false; // 有成员函数
                return instance;
            }
            Type* elmty = stack->tydef->type->elmget(name);
            if (elmty) { // 找到成员
                AST* value = build();
                // 类型检查
                if ( ! getType(value)->is(elmty)) {
                    FATAL("member assign type not match !")
                }
                stack->fundef->is_static_member = false; // 有成员函数
                // 返回类成员赋值
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
    Type* vty = getType(value);

    // 类型检查
    if (!vty->is(ty)) {
        FATAL("can't assignment <"
            +vty->getIdentify()+"> to var "
            +name+"<"+ty->getIdentify()+">  !")
    }

    // 添加变量到栈
    // stack->set(name, new ElementVariable(vty));

    return new ASTVariableAssign( name, value );

}

/**
 * type 声明类型
 */
AST* Build::build_type()
{
    if (stack->tydef) { // 嵌套定义
        FATAL("can't define type in type define !")
    }
    
    Word word = getWord();

    if (NOTWS(Character)) {
        FATAL("type define need a legal name to belong !")
    }

    string typeName = fixNamespace( word.value );

    // 查询类型是否定义
    if (auto *fd = dynamic_cast<ElementType*>(stack->find(typeName))) {
        FATAL("can't repeat type '"+typeName+"' !")
    }

    word = getWord();
    if (NOTSIGN("(")) {
        FATAL("namespace define need a sign ( to belong !")
    }

    // 新建类型
    TypeStruct* tyclass = new TypeStruct(typeName);
    if (stack->fundef) {
        tyclass->increment(); // 函数定义名称加上自增索引
    }

    // 生成AST
    ASTTypeDefine* tydef = new ASTTypeDefine();
    tydef->type = tyclass;

    auto *elmty = new ElementType(tyclass);

    // 新栈
    Stack *old_stk = stack;
    Stack *new_stk = new Stack(stack);
    // 添加到当前栈帧 支持函数参数
    new_stk->put(typeName, elmty);
    new_stk->tydef = tydef;
    new_stk->fundef = nullptr; // 类成员函数不属于任何包裹函数
    stack = new_stk;


#define TCADD(N) if(it) tyclass->add(N, it);

    while (1) { // 类型元素定义
        Type* it = nullptr;
        word = getWord();
        if (ISSIGN(")")) {
            TCADD("")
            break; // 类型声明结束
        }
        // 类函数定义
        AST* fundef(nullptr);
        if("fun"==word.value){
            fundef = build_fun();
        } else if ("dcl" == word.value) {
            fundef = build_dcl();
        }
        if (fundef) {
            continue; // 成员函数定义
        }
        // 类型标示
        Element* res = stack->find(word.value);
        if (ElementType* dco = dynamic_cast<ElementType*>(res)) {
            it = dco->type;
        } else {
            FATAL("Type declare format error: not find Type <"<<word.value<<"> !")
        }
        // 类型标记
        word = getWord();
        res = stack->find(word.value);
        if (ElementType* dco = dynamic_cast<ElementType*>(res)) {
            prepareWord(word); // 预备
            TCADD("")
        } else if(ISWS(Character)) {
            TCADD(word.value)
        } else {
            FATAL("Type declare format error !")
        }
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
 * dcl 声明函数
 */
AST* Build::build_dcl()
{
    Word word = getWord();
    if (NOTWS(Character)) {
        FATAL("function declare need a type name to belong !")
    }

    // 函数返回值类型
    Type *rty;
    Element* elmret = stack->find(word.value);
    if (auto *ety = dynamic_cast<ElementType*>(elmret)) {
        rty = ety->type;
    }
    else {
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
    word = getWord();
    if (NOTSIGN("(")) {
        FATAL("function declare need a sign ( to belong !")
    }

    // 函数参数解析
    while (true) {
        word = getWord();
        if (ISSIGN(")")) {
            break; // 函数参数列表结束
        }
        Type *pty;  // 参数类型
        if (auto *ety = dynamic_cast<ElementType*>(stack->find(word.value))) {
            functy->add("", ety->type);
        } else {
            FATAL("function declare parameter format is not valid !")
        }
    }
    
    // 函数是否已经声明
    ASTFunctionDefine* fundef = stack->findFunction(functy);

    // 判断函数是否已经定义
    if (!fundef) {
        fundef = new ASTFunctionDefine(functy, nullptr);
        // 添加新函数
        stack->addFunction(fundef);
    }

    // 返回函数声明
    return new ASTFuntionDeclare(fundef->ftype);
}

/**
 * fun 定义函数
 */
AST* Build::build_fun()
{
    Word word = getWord();
    if (NOTWS(Character)) {
        FATAL("function define need a type name to belong !")
    }

    // 函数返回值类型
    Type *rty(nullptr); // nullptr 时需要推断类型
    Element* elmret = stack->find(word.value);
    if (auto *ety = dynamic_cast<ElementType*>(elmret)) {
        rty = ety->type;
    } else {
        prepareWord(word);
    }

    // 函数名称
    word = getWord();
    if (NOTWS(Character)) {
        FATAL("function define need a legal name !")
    }

    // 新建函数类型
    string funcname = word.value;
    auto *functy = new TypeFunction(funcname);

    // 括号
    word = getWord();
    if (NOTSIGN("(")) {
        FATAL("function define need a sign ( to belong !")
    }

    // 函数参数解析
    while (true) {
        word = getWord();
        if (ISSIGN(")")) {
            break; // 函数参数列表结束
        }
        Type *pty;  // 参数类型
        string pnm; // 参数名称
        if (auto *ety = dynamic_cast<ElementType*>(
            stack->find(word.value))) {
            pty = ety->type;
        } else {
            FATAL("Parameter format is not valid !")
        }
        word = getWord();
        if (NOTWS(Character)) {
            FATAL("function parameter need a legal name !")
        }
        pnm = word.value;
        if (stack->tydef && pnm==DEF_MEMFUNC_ISTC_PARAM_NAME) {
            // 类成员函数，参数名称冲突
            FATAL("can't give a parameter name '"+pnm+"' in type member function !")
        }
        functy->add(pnm, pty);
    }

    // 函数是否已经声明
    ASTFunctionDefine* aldef = stack->findFunction(functy);
    ASTFunctionDefine *fundef;

    // 判断函数是否已经定义
    if (aldef){
        if (aldef->body) {
            // 函数以声明，且 body 已定义
            FATAL("function define repeat: " + functy->str());
        }
        fundef = aldef;
    } else {
        // 新建函数定义
        fundef = new ASTFunctionDefine(functy);
    }

    // 设置可能标记的返回值
    functy->ret = rty;
    fundef->ftype = functy;

    // 解析函数体

    // 创建新分析栈
    Stack* old_stack = stack;
    Stack new_stack(stack);
    new_stack.fundef = fundef; // 当前定义的函数
    // 提前 添加函数 支持递归
    new_stack.addFunction(fundef);

    // 函数定义环境
    fundef->wrap = stack->fundef; // wrap
    fundef->belong = stack->tydef; // belong

    // ElementStack nsk; // 缓存旧变量
    int i(0);
    for (auto &pty : functy->types) {
        string pn(functy->tabs[i]);
        new_stack.put(pn, new ElementVariable(pty)); // 加实参
        i++;
    }
    // 替换新栈帧
    stack = & new_stack;

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
        body->add( build() );
    }

    // 返回值验证与推断
    size_t bodylen = body->childs.size();
    Type *lastChildTy(nullptr);
    if(bodylen>0){
        // 获取函数体最后一句为返回类型
        AST* last = body->childs[bodylen - 1];
        lastChildTy = getType(last);
    }

    // 检查返回值类型一致性
    verifyFunctionReturnType(lastChildTy);

    if ( ! functy->ret) {
        cout << "! functy->ret" << endl;
        functy->ret = Type::get("Nil");
    }

    // 复位旧栈帧
    stack = old_stack;
    // 打印语法分析栈
    DEBUG_WITH("als_stack", \
        cout << endl << endl << "==== Analysis stack ( function "+funcname+" ) ===" << endl << endl; \
        new_stack.print(); \
        cout << endl << "====== end ======" << endl << endl; \
        )


    // 加上 Body
    fundef->body = body;

    // 函数已经声明过了，添加 body
    if (aldef) {
        // 替换 ftype，加入参数名称
        // delete aldef->ftype;
        // aldef->ftype = functy;

    // 否则添加新函数
    } else {
        stack->addFunction(fundef);
    }

    // 添加到类成员函数
    if(stack->tydef){
        stack->tydef->members.push_back(fundef);
    }

    // 返回值
    return fundef;
}

/**
 * ret 函数值返回
 */
AST* Build::build_ret()
{
    AST *ret = build();
    // 验证返回值
    verifyFunctionReturnType( getType(ret) );
    return new ASTRet(ret);
}

/**
 * tpf 模板函数推断
 */
AST* Build::build_tpf()
{
    Word word = getWord();

    if (NOTWS(Character)) {
        FATAL("template function define need a legal name to belong !")
    }

    string tpfName = word.value; // fixNamespace(word.value);
    if (stack->find(DEF_SPLITFIX_TPFPREFIX + tpfName)) {
        FATAL("template function duplicate definition '"+tpfName+"' !");
    }

    word = getWord();
    if (NOTSIGN("(")) {
        FATAL("template function define need a sign ( to belong !")
    }

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
    cacheWordSegment(tpfdef->bodywords); // 缓存括号段内容
    
    // 添加到分析栈
    stack->put(DEF_SPLITFIX_TPFPREFIX + tpfName, new ElementTemplateFuntion(tpfdef));

    return tpfdef;
}

/**
 * if 条件分支
 */
AST* Build::build_if()
{

#define DO_FATAL FATAL("No match bool function Type<"+idn+"> when be used as a if condition")
    
    AST* cond = build();
    Type* cty = getType(cond);
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

    // if 语句
    astif->pthen = build();

    // 检查 else
    auto word = getWord();
    if (ISCHA("else")) {
        astif->pelse = build();
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
    Type* cty = getType(cond);
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

    // while 语句
    astwhile->body = build();

    // 返回 if 节点
    return astwhile;

}

/**
 * let 符号绑定
 */
AST* Build::build_let()
{
    auto word = getWord();
    if (NOTSIGN("(")) {
        FATAL("let binding need a sign ( to belong !")
    }

    // 唯一名称
    string idname("");
    bool sign = false;

    auto *let = new ElementLet();
    auto *relet = new ASTLet();

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
            idname += "_";
        } else if (ISWS(Operator)) {
            idname += str;
            sign = true;
        } else {
            FATAL("let unsupported the symbol type '"+str+"' !")
        }
    }

    if (!sign) { // 至少提供一个符号
        FATAL("let binding must provide at least one symbol !")
    }

    auto *fd = stack->find(idname);
    if (fd) { // 不能重复绑定
        FATAL("let can't repeat binding '"+idname+"' !")
    }
    
    word = getWord();
    if (NOTSIGN("(")) {
        FATAL("let binding need a sign ( to belong !")
    }

    // 绑定体
    while (true) {
        word = getWord();
        if (ISSIGN(")")) {
            break; // 体结束
        }
        relet->body.push_back(word.value);
        let->bodywords.push_back(word);
    }

    // 添加栈到绑定
    stack->put(idname, let);

    // 返回
    return relet;
}

/**
 * let 符号绑定展开
 *
AST* Build::spreadLetBind(list<Word>*pwds)
{
    string idname("");

    // 参数
    vector<Word> params;
    Word placeholder;
    if (pwds) { // 上层存在，并且连续绑定
        idname = "_";
        params.push_back(placeholder);
    }

    list<Word> caches;
    
    ElementLet* let(nullptr);
    //ElementLet* macth(nullptr);
    //string stkn("");

    filterLet* filter(nullptr);

    while (true) {
        auto word = getWord();
        caches.push_back(word);
        if (ISWS(Operator)) {
            idname += word.value;
        }
        else {
            idname += "_";
            params.push_back(word);
        }
        if (!filter) {
            filter = new filterLet(stack, idname);
            continue;
        }
        int num = filter->match(idname);
        if (filter->unique) {
            let = filter->unique;
            goto macth_success; // 找到唯一匹配
        }
        if (num==0) {
            FATAL("let bind can't find !")
        }
    }

    FATAL("let bind can't find until the end !")


    macth_success:

    // 解析参数
    map<string, Word> pmstk;
    int i = 0;
    for (auto &p : let->params) {
        pmstk[p] = params[i];
        i++;
    }

    // 展开参数
    list<Word> results;
    for (auto &word : let->bodywords) {
        string str(word.value);
        if (ISWS(Character) && pmstk.find(str) != pmstk.end()) {
            auto wd = pmstk[str];
            if (wd == placeholder) {
                for (auto &p : *pwds) {
                    // 上层解析的内容替换
                    results.push_back(p);
                }
            }
            else {
                results.push_back(wd); // 参数替换
            }
        }
        else {
            results.push_back(word);
        }
    }

    auto word = getWord();

    // 查看接下来是否需要继续绑定符号
    if (ISWS(Operator)) {
        // 继续绑定
        prepareWord(word);
        return spreadLetBind(&results);
    } else {

        cout << "spreadLetBind【";
        for (auto &p : results) {
            cout << " " << p.value;
        }
        cout << "】" << endl;


        // 符号绑定结束
        prepareWord(word);
        // 预备
        prepareWord(results);
        // 重新开始
        return build();

    }









}
*/
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
        tyclass  = dynamic_cast<TypeStruct*>(getType(val));
    }

    if( ! tyclass){
        FATAL("elmivk must belong a struct value '"+word.value+"' !")
    }

    // 查询目标类成员函数
    word = getWord();
    auto target_stack = type_member_stack[tyclass];
    // 成员函数调用，不向上查找
    auto filter = filterFunction(target_stack, word.value, false);
    if (!filter.size()) {
        FATAL("can't find member function '"
            +word.value+"' in class '"+tyclass->name+"'")
    }


    string name = word.value;
    auto *fncall = new ASTFunctionCall(nullptr);
    // auto elms = grp->elms;

    // 节点缓存
    //list<AST*> cachebuilds;
        
    // string fname(name); // 函数参数组装名
    // vector<Element*> matchs; // 匹配的函数
    // ElementFunction* elmfn;
    ASTFunctionDefine* fundef(nullptr);

    // 临时用来查询函数的类型
    auto *tmpfty = new TypeFunction(name);


    // 解析成员函数调用

        // 采用贪婪匹配模式
    while (true) {
        // 匹配函数（第一次无参）
        int match = filter.match(tmpfty);
        if (match==1 && filter.unique) {
            fundef = filter.unique; // 找到唯一匹配
            string idname = fundef->ftype->getIdentify();
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
                fundef = filter.unique; // 找到唯一匹配
                break;
            }
        }
        // 添加参数
        AST *exp = build();
        if (!exp) {
            if (filter.unique) {
                fundef = filter.unique; // 无更多参数，找到匹配
                break;
            }
            FATAL("No macth function '"+tyclass->name+"."+tmpfty->getIdentify()+"' !");
        }
        //cachebuilds.push_back(exp);
        fncall->addparam(exp); // 加实参
        auto *pty = getType(exp);
        // 重载函数名
        tmpfty->add("", pty);
    }



    /*
    while (1) {
        AST *exp;
        auto word = getWord();
        if (ISSIGN(";")) { // 函数调用结束
            int match = filter.match(tmpfty);
            if(filter.unique){
                fundef = filter.unique;
                break;
            }
            if (match==0) {
                prepareBuild(cachebuilds); // 复位缓存节点
                throw "No macth function !";
                // FATAL("No macth function !")
            }
        } else {
            prepareWord(word);
        }
        // 匹配函数（第一次无参）
        int match = filter.match(tmpfty);
        if(filter.unique){
            fundef = filter.unique;
            break;
        }
        if (match==0) {
            prepareBuild(cachebuilds); // 复位缓存节点
            throw "No macth function !";
            // FATAL("No macth function !")
        }
        // 添加参数
        exp = build();
        cachebuilds.push_back(exp);
        fncall->addparam(exp); // 加实参
        auto *pty = getType(exp);
        // 重载函数名
        tmpfty->add("", pty);
    }
    */

    fncall->fndef = fundef; // elmfn->fndef;
        
    // return fncall;





    /*


    // 启动新栈
    auto old_stack = stack;
    stack = type_member_stack[tyclass];

    // 调用函数
    AST* res = build();
    auto* fcall  = dynamic_cast<ASTFunctionCall*>(res);
    if( ! fcall){
        FATAL("elmivk must belong a member function call !")
    }
    // 复位栈
    stack = old_stack;

    
    */


    // 静态成员函数验证
    if (is_static && ! fundef->is_static_member) {
        FATAL("'"+fundef->ftype->name+"' is not a static member function !")
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
    auto* scty = dynamic_cast<TypeStruct*>(getType(sctval));
    if( ! scty ){
        FATAL("elmget must eat a Struct value !")
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

    return ins;
}

/**
 * member set 成员赋值
 */
AST* Build::build_elmset()
{
    auto *ins = new ASTMemberAssign();

    AST* sctval = build();
    auto* scty = dynamic_cast<TypeStruct*>(getType(sctval));
    if( ! scty ){
        FATAL("elmget must eat a Struct value !")
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
    Type* putty = getType(putv);
    // 类型检查
    Type* pvty = scty->types[ins->index];
    if (!putty->is(pvty)) {
        FATAL("can't member assign <"+pvty->str()+"> by <"+putty->str()+">' !")
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
            cacheWordSegment(cache); // 缓存括号段内容
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
        idname = "_";
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
            cacheWordSegment(cache); // 缓存括号段内容
            if (cache.empty()) {
                FATAL("Operator binding priority error !")
            }
            params.push_back(cache);
            idname += "_";
        }else if (ISWS(Operator)) { // 操作符
            idname += word.value;
        } else {
            idname += "_";
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
        return spreadOperatorBind(&results);
    }


    // 符号绑定结束
    return results;
}



/**
 * 缓存单词段（包含括号内部所有内容）
 */
void Build::cacheWordSegment(list<Word>& cache)
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
}





}
}
