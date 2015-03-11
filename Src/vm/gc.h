#ifndef DEF_GC_H
#define DEF_GC_H

/**
 * Def 对象分配及垃圾回收
 */

#include <string>
#include <list>

#include "../object/object.h"
#include "../parse/node.h"

using namespace std;

using namespace def::object;
using namespace def::node;

namespace def {
namespace gc {

// 空闲对象列表
typedef list<DefObject*> FreeObjs;

// 对象分配及回收
class Gc{

	FreeObjs free_int;      // 空闲 int 列表
	FreeObjs free_float;    // 空闲 float 列表
	FreeObjs free_string;   // 空闲 string 列表

	ObjectNone* prep_none;      // none 小对象
	ObjectBool* prep_true;      // true 小对象
	ObjectBool* prep_false;     // false 小对象
	ObjectInt*  prep_ints[270];      // int    小对象池  -10～260
	//ObjectNone* prep_strings[52];   // string 小对象池 a~z A~Z  // 52=26+26

	public:

	Gc();

	ObjectBool* AllotBool(bool);   // 分配 bool 对象
	ObjectInt* AllotInt(long);     // 分配 int 对象
	DefObject* Allot(Node*);       // 从Node语法节点分配新的对象
	DefObject* Quote(DefObject*);  // 引用对象
	bool Free(DefObject*);   // 释放对象
	bool Recycle(DefObject*);      // 回收对象

};


} // --end-- namespace gc
} // --end-- namespace def

#endif
// --end-- DEF_STACK_H

