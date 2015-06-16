#ifndef DEF_VM_EXEC_H
#define DEF_VM_EXEC_H

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
#include "envir.h"

using namespace std;

using namespace def::parse;


namespace def {
namespace vm {

// 调用
class Exec {

	public:

	Envir _envir; // 当前调用执行环境

	public:

	Exec();
	Exec(Envir e); // 拷贝执行环境构造

	//~Exec(); // 析构

	void StackPush(string, DefObject*); // 执行栈变量初始化
	inline Stack* StackParent(Stack*p=NULL);           // 指定&获取父栈

	// 支持
	inline bool Free(DefObject*);      // 变量的解引用
	inline ObjectNone* NewObjNone();   // 返回 none 对象
	inline ObjectBool* NewObjTrue();   // 返回 true 对象
	inline ObjectBool* NewObjFalse();  // 返回 false 对象

	// 执行
	bool Main(string); // 从入口文件开始执行
	static Node* Parse(string &text, string file=""); // 解析文本得到抽象语法树
	bool Run(); // 执行调用帧

	// 求值
	DefObject* Evaluat(Node*);  // 对节点求值操作

	// 节点求值
	DefObject* Assign(Node*);   // 赋值操作
	DefObject* AssignUp(Node*); // 向上搜索赋值

	DefObject* Operate(Node*, Node*, NodeType); // 算法操作 + - * /
	// DefObject* Equal(Node*, Node*);      // 等于 =
	// DefObject* More(Node*, Node*);       // 大于 >
	// DefObject* Less(Node*, Node*);       // 小于 <
	// DefObject* MoreEqual(Node*, Node*);  // 大于等于 >=
	// DefObject* LessEqual(Node*, Node*);  // 小于等于 <=
	// DefObject* NotEqual(Node*, Node*);   // 不等于 ~=
	// DefObject* Not(Node*, Node*);        // bool取反 ~

	DefObject* If(Node*); // if 结构
	DefObject* While(Node*); // while 结构

	DefObject* List(Node*);  // list 列表
	DefObject* Dict(Node*);  // dict 字典
	DefObject* Block(Node*); // block 块
	
	DefObject* ProcDefine(Node*); // 处理器定义
	DefObject* FuncDefine(Node*); // 函数定义

	DefObject* ContainerAccess(Node*); // 容器访问
	DefObject* MemberAccess(Node*);    // 成员访问
	
	DefObject* Import(Node*);        // 模块加载
	DefObject* Import(string name);        // 模块加载
	ObjectModule* CreateModule(string); // 模块创建

	DefObject* Print(Node*); // 打印操作

};




} // --end-- namespace vm
} // --end-- namespace def

#endif
// --end-- DEF_VM_EXEC_H
