#ifndef DEF_EXEC_H
#define DEF_EXEC_H

/**
 * Def 执行上下文（函数、处理器调用环境）
 */


#include <string>

#include "../parse/nodezer.h"
#include "../object/object.h"
#include "../object/operat.h"


#include "gc.h"
#include "stack.h"
#include "module.h"

using namespace std;

using namespace def::parse;


namespace def {
namespace vm {


// 调用栈类型
enum class ExecType{
	Main,    // 主入口调用
	Module,  // 模块调用
	Func,    // 函数调用
	Proc,    // 处理器调用
};



// 调用栈
class Exec {

	public:

	ExecType type;    // 调用栈类型

	Node* _node;      // 抽象语法树
	Stack* _stack;    // 执行栈
	Gc* _gc;          // 对象分配及垃圾回收
	Module* _mod;     // 模块管理

	public:

	Exec(Node*, Gc*, Module*, ExecType=ExecType::Main);
	Exec(Exec&, ExecType=ExecType::Main); // 拷贝构造
	void StackPush(string, DefObject*); // 执行栈变量初始化
	inline Stack* StackParent(Stack*p=NULL);           // 指定&获取父栈


	// 支持
	inline bool Free(DefObject*);      // 变量的解引用或垃圾回收
	inline ObjectNone* NewObjNone();   // 返回 none 对象
	inline ObjectBool* NewObjTrue();   // 返回 true 对象
	inline ObjectBool* NewObjFalse();  // 返回 false 对象

	// 
	DefObject* Run();  // 执行调用帧

	// 执行求值
	DefObject* Evaluat(Node*);  // 对节点求值操作

	// 节点求值
	DefObject* Assign(Node*);   // 赋值操作
	DefObject* AssignUp(Node*); // 向上搜索赋值

	DefObject* Print(Node*); // 打印操作
	DefObject* Print(DefObject*); // 打印操作

	DefObject* Operate(Node*, Node*, NodeType); // 算法操作

	DefObject* If(Node*); // if 结构
	DefObject* While(Node*); // while 结构

	DefObject* List(Node*);  // list 列表
	DefObject* Dict(Node*);  // dict 字典
	DefObject* Block(Node*); // block 块
	
	DefObject* ProcDefine(Node*); // 处理器定义
	DefObject* FuncDefine(Node*); // 函数定义

	DefObject* ContainerAccess(Node*); // 容器访问
	// DefObject* MemberAccess(Node*);    // 成员访问
	
	DefObject* Import(Node*);    // 模块加载



};




} // --end-- namespace vm
} // --end-- namespace def

#endif
// --end-- DEF_EXEC_H
