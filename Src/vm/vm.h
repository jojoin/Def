#ifndef DEF_VM_H
#define DEF_VM_H

/**
 * Def 虚拟机（解释器）
 */


#include <string>

#include "../parse/nodezer.h"
#include "../object/object.h"
#include "../object/operat.h"

#include "module.h"
#include "gc.h"
#include "stack.h"
#include "exec.h"

using namespace std;

using namespace def::parse;
using namespace def::object;

namespace def {
namespace vm {


// 虚拟机
class Vm {

	public:

	Module* _mod;     // 模块管理
	Gc* _gc;     // 对象分配及垃圾回收
	// Exec* _exec; // 执行上下文

	public:

	Vm();

	bool Error(int); // 运行时错误

	bool Eval(string, bool); // 执行 Def 脚本


};


} // --end-- namespace vm
} // --end-- namespace def

#endif
// --end-- DEF_VM_H