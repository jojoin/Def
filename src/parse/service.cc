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
 * 构造
 */
Service::Service(Tokenizer * t)
    : tkz(t)
{
    // 初始化分析栈
    stack = new Stack(nullptr);
}
    


/**
 * 比较函数返回值类型
 */
void Service::verifyFunctionReturnType(Type* ret)
{
    auto *fndef = stack->fundef;
    if ( ! fndef) {
        FATAL("Non existence function cannot return value !")
    }
    if ( ret && ! fndef->ftype->ret) {
        fndef->ftype->ret = ret;
        return;
    }
    if (ret && ! ret->is(fndef->ftype->ret)) {
        // 函数返回值参数不匹配
        FATAL("Function '"+fndef->ftype->name+"' return value type not match !")
    }
}



/**
 * 查询名字
 * up: 是否向上查找
 */
string Service::fixNamespace(const string & name)
{
    if (defspace=="") {
        return name;
    }

    return defspace + "_" + name;
}





/**
 * 检查并设置 include 文件的绝对路径
 */
bool Service::checkSetInclude(const string& path)
{
    if (includes.find(path) == includes.end()) {
        includes.insert(path);
        return false;
    }
    // 已包含
    return true;
}


    
/**
 * 获取一个单词
 */
Tokenizer::Word Service::getWord()
{
    // 返回预备
    if (!prepare_words.empty()) {
        auto rt = prepare_words.front();
        prepare_words.pop_front();
        return rt;
    }

    // 获取新词
    Tokenizer::Word word  = tkz->gain();
    // 调试打印 token list
    DEBUG_WITH("tok_list", cout << word.value << " , ";)

    return word;
}


/**
 * 缓存一个单词到预备列表
 */
void Service::prepareWord(const Tokenizer::Word & wd)
{
    prepare_words.push_front(wd);
}


/**
 * 拷贝一个列表到预备列表
 */
void Service::prepareWord(list<Tokenizer::Word> wds)
{
    // 拷贝 并 将新的预备表插入到开头
    prepare_words.splice(prepare_words.begin(), wds);
}



/**
 * 类型判断
 */
bool Service::checkType(Type* type, AST* ast)
{
    return type->is(getType(ast));
}


/**
 * 类型判断
 */
Type* Service::getType(AST* ast)
{
#define ISAST(T) AST##T* con = dynamic_cast<AST##T*>(ast)
    
    // 常量类型
    if (ISAST(Constant)) {
        return con->type;
    }

    // 变量类型
    if (ISAST(Variable)) {
        return con->type;
    }

    // 变量定义
    if (ISAST(VariableDefine)) {
        return getType( con->value );
    }

    // 变量赋值
    if (ISAST(VariableAssign)) {
        return getType( con->value );
    }

    // 类型构造
    if (ISAST(TypeConstruct)) {
        return con->type;
    }
    
    // 函数类型
    if (ISAST(FunctionCall)) {
        return con->fndef->ftype->ret;
    }

    // 函数返回值
    if (ISAST(Ret)) {
        return getType( con->value );
    }

    // 成员函数调用
    if (ISAST(MemberFunctionCall)) {
        return getType( con->call );
    }

    // 成员访问
    if (ISAST(MemberVisit)) {
        TypeStruct* scty = (TypeStruct*)getType( con->instance );
        return scty->types[con->index];
    }

    // 成员赋值
    if (ISAST(MemberAssign)) {
        return getType( con->value );
    }


    return nullptr;
#undef ISAST
}




