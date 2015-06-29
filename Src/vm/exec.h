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
#include "throw.h"

using namespace std;

using namespace def::parse;


namespace def {
namespace vm {



#define DO DefObject

// 调用
class Exec {

	public:

	Envir _envir; // 当前调用执行环境

	public:

	Exec();
	Exec(Envir e); // 拷贝执行环境构造

	//~Exec(); // 析构

	void StackPush(string, DO*); // 执行栈变量初始化
	inline Stack* StackParent(Stack*p=NULL);           // 指定&获取父栈

	// 支持
	inline void VarPut(string,DO*);      // 变量的入栈（带垃圾回收）
	inline ObjectNone* ObjNone();   // 返回 none 对象
	inline ObjectBool* ObjTrue();   // 返回 true 对象
	inline ObjectBool* ObjFalse();  // 返回 false 对象

	// 执行
	bool Main(string); // 从入口文件开始执行
	static Node* Parse(string &text, string file=""); // 解析文本得到抽象语法树
	DO* Run(); // 执行调用帧
	DO* Eval(string); // 执行代码

	// 求值
	DO* Evaluat(Node*);  // 对节点求值操作

	// 节点求值
	DO* Variable(string);    // 取变量值
	DO* Assign(Node*);       // 赋值操作
	DO* Assign(Node*,DO*);   // 赋值操作
	DO* AssignUp(Node*); // 向上搜索赋值

	DO* Operate(Node*, Node*, NodeType); // 算法操作 + - * /
	DO* Compare(Node*, Node*, NodeType); // 比较 = > < >= <= ~=
	DO* Not(Node*);  // bool取反 ~

	DO* If(Node*); // if 结构
	DO* While(Node*); // while 结构

	DO* List(Node*);  // list 列表
	DO* Dict(Node*);  // dict 字典
	DO* Block(Node*); // block 块
	
	DO* ProcDefine(Node*); // 处理器定义
	DO* FuncDefine(Node*); // 函数定义
	DO* ProcCall(Node*);   // 处理器调用
	DO* FuncCall(Node*);   // 函数调用
	DO* Return(Node*);     // 函数返回
	void BuildProcArgv(Node*,Node*,Stack*); // 处理参数列表
	void BuildFuncArgv(Node*,Node*,Stack*); // 处理参数列表

	DO* ContainerAccess(Node*); // 容器访问
	DO* MemberAccess(Node*);    // 成员访问
	DO* MemberAccess(DO*, string);    // 成员访问
	
	DO* Import(Node*);        // 模块加载
	DO* Import(string name);        // 模块加载
	ObjectModule* CreateModule(string); // 模块创建

	DO* Print(Node*); // 打印操作

	// 系统服务
	DO* Sysfunc(string, Node*); // 调用系统函数（调用失败返回 NULL ）
	DO* Objfunc(Node*, Node*);
	DO* Objfunc(DO*, string, Node*); // 调用变量自带函数（调用失败返回 NULL ）


};




#undef DO









} // --end-- namespace vm
} // --end-- namespace def

#endif
// --end-- DEF_VM_EXEC_H
