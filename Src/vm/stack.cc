/**
 * Def 执行栈
 */

#include <iostream>
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
DefObject* Stack::Put(string name, DefObject *obj)
{
	v_local[name] = obj;
}


/**
 * 取得变量
 */
DefObject* Stack::Get(string name)
{
	map<string, DefObject*>::iterator iter = v_local.find(name);
	if(iter!=v_local.end()){
	    return iter->second;
	}else{
		return NULL;
	}
}


/**
 * 注册新创建的变量，用于垃圾回收
 */
DefObject* Stack::Regist(DefObject* obj)
{
    cout<<"Stack::Regist()"<<endl;
	o_create.push_back(obj);
	return obj;
}

