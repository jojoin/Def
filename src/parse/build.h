#pragma once

/** 
 * Def 语法分析树
 */


#include <string>
#include <vector>
#include <map>

#include "../global.h"
#include "../util/str.h"

#include "../core/builtin.h"
#include "../core/type.h"
#include "../core/ast.h"
#include "./tokenizer.h"
#include "./service.h"
#include "./stack.h"


namespace def {
namespace parse {
    
using namespace std;
using namespace def::core;
using namespace def::util;





/**
 * Def ast 生成器
 */
class Build : public Service
{
    
public:

    // 创建分析树
    ASTGroup* createAST();

protected:
    
    Build(Tokenizer * t);

    // 上下文状态
    // bool ctx_type = false;  // 类型定义
    // bool ctx_fun  = false;  // 函数定义
    
protected:

    list<AST*> prepare_builds; // 缓存的节点

    void prepareBuild(AST*);
    void prepareBuild(const list<AST*> &);

    // 创建单个句子 cache 允许去缓存的节点名
    AST* build(bool spread=true); // spread = 不检测 let 绑定
    // 创建 Group
    ASTGroup* buildGroup();
    // 函数调用
    // AST* buildFunctionCall(const string &, ElementGroup*, bool istpf=false);
    // 解析模板函数
    AST* buildTemplateFuntion(const string &, ElementTemplateFuntion*);
    // 核心定义处理
    AST* buildCoreDefine(const string &);
    // 变量或类成员访问
    AST* buildVaribale(Element*, const string &n="");
    // 变量构造函数调用
    AST* buildConstruct(ElementType*, const string &n="");
    AST* buildMacro(ElementLet*, const string &);

protected:

    // let 符号绑定展开
    // AST* spreadLetBind(list<Tokenizer::Word>*pwds=nullptr);
    AST* buildOperatorBind();
    bool forecastOperatorBind(); // 预测是否需要解开符号绑定
    list<Tokenizer::Word> spreadOperatorBind(list<Tokenizer::Word>*pwds=nullptr);
    
protected:

    // 建立函数调用，up=是否向上查找
    ASTFunctionCall* _functionCall(const string &, Stack*, bool up=true);
    // 从函数头（声明或定义）建立函数类型，declare=是否为声明格式
    TypeFunction* _functionType(bool declare=false);

    // 缓存单词段（包含括号内部所有内容）
    void cacheWordSegment(list<Tokenizer::Word>&);


protected:

    /** def 核心定义列表 **/
#define T(N) AST* build_##N();
    BUILTIN_DEFINES_LIST(T)
#undef T

    
};


}
}

