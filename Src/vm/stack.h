#ifndef DEF_STACK_H
#define DEF_STACK_H

/**
 * Def 执行栈
 */


#include <string>
#include <map>
 

#include "../object/object.h"


using namespace std;
using namespace def::object;


namespace def {
namespace stack {

// Def 语言执行变量栈
struct Stack{

	map<string, DefObject*> v_local; // 本地变量列表
	Stack* parent;                   // 父栈
	//vector<Stack*> childs;         // 子栈列表

	Stack();
	void Put(string, DefObject*);   // 将变量入栈
	DefObject* Get(string);         // 取变量值

};



} // --end-- namespace stack
} // --end-- namespace def

#endif
// --end-- DEF_STACK_H