#ifndef DEF_VM_H
#define DEF_VM_H

/**
 * Def 虚拟机（解释器）
 */


#include <string>

#include "../parse/nodezer.h"
#include "../object/object.h"

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

	bool Eval(string, bool); // 执行 Def 脚本
	bool Execute(Node*);     // 解释执行语法树
	DefObject* GetValue(Node*);  // 对节点求值操作
	DefObject* OperateAdd(DefObject*, DefObject*); // 加法操作
	DefObject* OperatePrint(DefObject*); // 打印操作




};


} // --end-- namespace vm
} // --end-- namespace def

#endif
// --end-- DEF_VM_H