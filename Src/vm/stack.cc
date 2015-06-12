/**
 * Def 执行栈
 */

#include <iostream>
#include <string>
 
#include "stack.h"

using namespace std;

using namespace def::object;


namespace def {
namespace vm {



Stack::Stack(Stack*p)
	: parent( p )
{

}


/**
 * 将变量入栈
 */
DefObject* Stack::VarPut(string name, DefObject *obj)
{
	DefObject* old = NULL;
	map<string, DefObject*>::iterator iter = v_local.find(name);
	if(iter!=v_local.end()){
	    old = iter->second; // 查找变量
	}
	// 入栈
	v_local[name] = obj;

	return old;
}


/**
 * 取得变量
 */
DefObject* Stack::VarGet(string name)
{
	map<string, DefObject*>::iterator iter = v_local.find(name);
	if(iter!=v_local.end()){
	    return iter->second;
	}else{
		return NULL;
	}
}



/**
 * 搜索变量入栈（变量必须存在）
 * @param loc 是否也查找当前栈帧
 */
DefObject* Stack::VarPutUp(string name, DefObject *obj, bool loc=true)
{
	DefObject* exi = NULL;
	if(loc){
		exi = VarGet(name);
		if(exi){
			return VarPut(name, obj); // 当前入栈
		}
	}
	// 查询父栈
	if(parent){
		exi = parent->VarGetUp(name); // 查询父栈
		if(exi){
			return parent->VarPutUp(name, obj); // 向上入栈
		}

	}

	return NULL; // 向上赋值失败

}


/**
 * 向上变量取值（包含当前栈）
 */
DefObject* Stack::VarGetUp(string name)
{
	DefObject* loc = VarGet(name);
	if(loc){
		return loc; // 当前栈取得
	}
	if(parent){
		return parent->VarGetUp(name); // 查询父栈
	}
	return NULL; // 未找到
}


} // --end-- namespace vm
} // --end-- namespace def

