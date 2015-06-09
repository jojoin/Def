#ifndef DEF_VM_H
#define DEF_VM_H

/**
 * Def 虚拟机（解释器）
 */


#include <string>

#include "../parse/nodezer.h"
#include "../object/object.h"
#include "../object/operat.h"
#include "../module/error.h"

#include "gc.h"
#include "stack.h"

using namespace std;

using namespace def::node;
using namespace def::object;
using namespace def::stack;
using namespace def::gc;


namespace def {
namespace vm {


// 虚拟机
class Vm {

	public:

	Stack* vm_stack; //虚拟机执行栈
	Gc* vm_gc; //虚拟机对象分配及垃圾回收



	public:

	Vm();

	bool Error(int); // 运行时错误

	bool Eval(string, bool); // 执行 Def 脚本
	bool ExplainAST(Node*);     // 解释执行语法树
	inline bool Free(DefObject*);   // 变量的解引用或垃圾回收
	//bool Regist(DefObject*); // 登记新创建的变量，用于集中垃圾回收

	DefObject* Evaluat(Node*);  // 对节点求值操作

	DefObject* Print(Node*); // 打印操作
	DefObject* Print(DefObject*); // 打印操作

	DefObject* Operate(Node*, Node*, NodeType); // 算法操作

	DefObject* ControlIf(Node*); // if 结构
	DefObject* ControlWhile(Node*); // while 结构

	DefObject* StructList(Node*);  // list 列表
	DefObject* StructDict(Node*);  // dict 字典
	DefObject* StructBlock(Node*); // block 块
	
	DefObject* DefineProc(Node*); // 处理器定义
	DefObject* DefineFunc(Node*); // 函数定义

	DefObject* ContainerAccess(Node*); // 容器访问

	

	

	


};


} // --end-- namespace vm
} // --end-- namespace def

#endif
// --end-- DEF_VM_H