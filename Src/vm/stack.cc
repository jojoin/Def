/**
 * Def 执行栈
 */

#include <string>
 
#include "stack.h"

using namespace std;

using namespace def::object;
using namespace def::stack;


Stack::Stack()
	: parent(NULL)
{

}


/**
 * 将变量入栈
 */
void Stack::Put(string name, DefObject *obj)
{
	v_local[name] = obj;
}


/**
 * 取得变量
 */
DefObject* Stack::Get(string name)
{
	return v_local[name];
}
