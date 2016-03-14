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
    auto *fndef = stack->fndef;
    if ( ! fndef) {
        FATAL("Non existence function cannot return value !")
    }
    if (ret && status_construct) { // 构造函数不能有返回值
        FATAL("class construct function cannot have any return value !")
    }
    if ( ret && ! fndef->ftype->ret) {
        fndef->ftype->ret = ret;
        return;
    }
    if ( ret && ! ret->is(fndef->ftype->ret)) {
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
    DEBUG_WITH("tok_list", cout << word.value << " ";)
    DEBUG_WITH("token_list", cout << (int)word.state << " " << word.value << endl;)

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
    return type->is(ast->getType());
}



/**
 * 检查并重设 adt 状态
 */
bool Service::checkModADT()
{
    bool sta = is_mod_adt;
    is_mod_adt = false;
    return sta;
}

/**
 * 设置 adt 状态
 */
void Service::setModADT(bool s=true)
{
    is_mod_adt = s;
}
