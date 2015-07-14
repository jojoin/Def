#ifndef DEF_VM_GC_H
#define DEF_VM_GC_H

/**
 * Def 对象分配及垃圾回收
 */

#include <string>
#include <list>

#include "../object/object.h"
#include "../parse/node.h"
#include "./stack.h"

using namespace std;

using namespace def::parse;
using namespace def::object;

namespace def {
namespace vm {

// 空闲对象列表
typedef list<DefObject*> ObjList;

// 对象分配及回收
class Gc{

	public:

	// ObjList free_int;      // 空闲 int 列表
	// ObjList free_float;    // 空闲 float 列表
	// ObjList free_string;   // 空闲 string 列表

	ObjectNil*  prep_nil;       // nil 小对象
	ObjectBool* prep_true;      // true 小对象
	ObjectBool* prep_false;     // false 小对象
	ObjectInt*  prep_ints[270]; // int    小对象池  -10～260
	ObjectString*  prep_empty_str; // string 空字符串
	//ObjectNil* prep_strings[52];   // string 小对象池 a~z A~Z  // 52=26+26

	Gc();

	ObjectBool* AllotBool(bool);   // 分配 bool 对象
	ObjectInt* AllotInt(long);     // 分配 int 对象
	ObjectFloat* AllotFloat(double);     // 分配 float 对象
	ObjectString* AllotString(string);     // 分配 string 对象

	ObjectList* AllotList();       // 分配 list 对象
	ObjectDict* AllotDict();       // 分配 dict 对象
	ObjectBlock* AllotBlock();     // 分配 block 对象

	ObjectProc* AllotProc(Node*);     // 分配 proc 对象
	ObjectFunc* AllotFunc(Node*);     // 分配 func 对象
	ObjectNode* AllotNode(Node*);     // 分配 node 对象

	DefObject* Allot(Node*);       // 从Node语法节点分配新的对象

	bool Quote(DefObject*);      // 引用对象
	bool Free(DefObject*);       // 释放对象

};


} // --end-- namespace vm
} // --end-- namespace def

#endif
// --end-- DEF_VM_GC_H

