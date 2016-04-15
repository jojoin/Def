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
 * 获取一个单词
 */
Stack::Stack(Stack* p)
    : parent(p)
{
    // 顶级分析栈则初始化
    if (!p) {
        Initialize();
    } else {
        tydef = p->tydef;
        fndef = p->fndef;
    }

}


// 初始化基础分析栈
void Stack::Initialize()
{
    // 添加原子类型到栈内
#define AT(T) stack[#T] = new ElementType(Type::get(#T));
    DEF_AOTM_TYPE_LIST(AT)
#undef AT

    // 添加内建函数
#define BC(N,P) addBuiltinFunction(#N "," P);
    BUILTIN_FUNCTION_LIST(BC,BC)
#undef BC

}

/**
 * 添加内建函数
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
    // 内建函数的函数体为空
    // cout << fty->str() << endl;
    addFunction(fty, nullptr);
}


/**
 * 添加函数
 */
void Stack::addFunction(TypeFunction* fty, ASTGroup* body)
{
    ASTFunctionDefine* fndef = new ASTFunctionDefine(fty, body);
    addFunction(fndef);
}


/**
 * 添加函数
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
        stack[fty->name] = addgp; // 首次初始化
    }
    // 添加函数
    
    string fntylist = fty->getIdentify();

    auto *intaddfunc = new ElementFunction(fndef);
    addgp->elms.insert(make_pair(fntylist, intaddfunc));
    // 完成
}



/**
 * 查询名字
 * up: 是否向上查找
 */
ASTFunctionDefine* Stack::findFunction(TypeFunction* ftype)
{
    string fname = ftype->name;
    // 判断函数是否已经声明
    if (auto *fngr = dynamic_cast<ElementGroup*>(find(fname))) {
        string fidn = ftype->getIdentify();
        auto fd = fngr->elms.find(fidn);
        if (fd != fngr->elms.end()) {
            if (auto *declare = dynamic_cast<ElementFunction*>(fd->second)) {
                // 函数已经声明
                return declare->fndef;
            }
        }
    }

    // 查找父级栈
    if(parent){
        return parent->findFunction(ftype);
    }

    // 未定义
    return nullptr;

}





/**
 * 打印分析栈
 */
void Stack::print()
{
    for (auto &li : stack) {
        if (!li.second) continue;
        cout << li.first << ": " << li.second->str() << endl;
    }
}

/**
 * 查询名字
 * up: 是否向上查找
 */
Element* Stack::find(const string & name, bool up)
{
    ElementStack::iterator it;
    it = stack.find(name);
    if(it!=stack.end()){
        return it->second; // 找到
    }
    // 加上名字空间再查找
    it = stack.find(name);
    if(it!=stack.end()){
        return it->second; // 找到
    }
    // 查询父栈
	if(up&&parent){
        auto res = parent->find(name);
        // 记录函数捕获的外层变量
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
 * 当前块添加名字
 * return: 旧名字值
 */
Element* Stack::put(const string & name, Element* elem)
{
    ElementStack::iterator it;
    it = stack.find(name);
    if(it==stack.end()){
        stack.insert(ElementStack::value_type(name, elem));
        return nullptr; // 插入新名字
    }
    Element* old = it->second;
    stack[name] = elem;
    return old; // 返回旧名字值
}


/**
 * 设置名字
 * 变量必须已经存在
 * up: 是否向上查找设置
 * return: 旧名字=成功  NULL=失败
 */
Element* Stack::set(const string & name, Element* elem, bool up)
{
    ElementStack::iterator it;
    it = stack.find(name);
    if(it!=stack.end()){ // 找到并设置
        Element* old = it->second;
        stack[name] = elem;
        return old;
    }
    if(up&&parent){
        return parent->set(name, elem, true); // 查父块
    }
    return nullptr; // 没找到
}

