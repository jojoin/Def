#ifndef DEF_STACK_H
#define DEF_STACK_H

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
namespace stack {

// Def 语言执行变量栈
struct Stack{

	map<string, DefObject*> v_local; // 本地变量名字表
	list<DefObject*> o_create;       // 本地新创建的变量（用于垃圾回收）
	Stack* parent;                   // 父栈
	//vector<Stack*> childs;         // 子栈列表

	Stack();
	size_t Regist(DefObject*);           // 登记新创建的值
	DefObject* VarPut(string, DefObject*);   // 将变量入栈
	DefObject* VarGet(string);         // 取变量值（不存在返回NULL）

};



} // --end-- namespace stack
} // --end-- namespace def

#endif
// --end-- DEF_STACK_H