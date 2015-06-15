#ifndef DEF_VM_ENVIR_H
#define DEF_VM_ENVIR_H

/**
 * Def 执行环境包
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
using namespace def::vm;

namespace def {
namespace vm {


// 调用栈类型
enum class EnvirType{
	Main,    // 主入口调用
	Module,  // 模块调用
	Func,    // 函数调用
	Proc,    // 处理器调用
};




// 执行环境统一包装
class Envir {

	public:

	EnvirType _type;    // 调用类型
	string    _file;    // 调用执行所在文件

	Module* _module;   // 模块加载管理
	Gc*     _gc;       // 对象分配及垃圾回收
	Stack*  _stack;    // 执行栈帧
	Node*   _node;     // 目标抽象语法树

	public:

	// 完全初始化
	Envir(void){
		_file   = "";
		_type   = EnvirType::Main;
		_module = new Module();
		_gc     = new Gc();
		_stack  = new Stack();
	}
	// 拷贝构造
	Envir(const Envir &e){
		_type   = e._type;
		_module = e._module;
		_gc     = e._gc;
		_stack  = e._stack;
		_node   = e._node;
	}
	// 设置
	inline void Set(EnvirType o){ _type   = o; }
	inline void Set(Module   *o){ _module = o; }
	inline void Set(Gc       *o){ _gc     = o; }
	inline void Set(Stack    *o){ _stack  = o; }
	inline void Set(Node     *o){ _node   = o; }
	inline void SetFile(string &o){ _file = o; }
	// 全部析构
	void Clear(){
		delete _module;
		delete _gc;
		delete _stack;
	}

};


} // --end-- namespace vm
} // --end-- namespace def


#endif
// --end-- DEF_VM_ENVIR_H