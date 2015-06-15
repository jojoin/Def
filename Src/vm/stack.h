#ifndef DEF_VM_STACK_H
#define DEF_VM_STACK_H

/**
 * Def 执行栈
 */


#include <string>
#include <map>
#include <list>
 

#include "../object/object.h"


using namespace std;
using namespace def::object;


namespace def {
namespace vm {

// Def 语言执行变量栈
struct Stack{

	map<string, DefObject*> v_local; // 本地变量名字表
	Stack* parent;                   // 父栈

	public:

	Stack(Stack*p=NULL);

	// 变量操作
	DefObject* VarPut(string, DefObject*);   // 将变量入当前栈（返回旧变量，不存在返回NULL）
	DefObject* VarGet(string);         // 于当前栈取变量值（不存在返回NULL）

	DefObject* VarPutUp(string, DefObject*, bool);   // 向上查找变量并入栈（返回旧变量，不存在返回NULL）
	DefObject* VarGetUp(string);   // 向上查找取得变量值

};



} // --end-- namespace vm
} // --end-- namespace def


#endif
// --end-- DEF_VM_STACK_H