/** 
 * Def 对象 操作
 */


#include "operat.h"

using namespace std;

using namespace def::operat;
using namespace def::object;

/**
 * 转换为 bool 类型
 */
bool Conversion::Bool(DefObject *obj)
{

	if(obj==NULL) return false;

#define T ObjectType

	T t = obj->type;

	if(t==T::None){
		return false;
	}else if(t==T::Bool){
		ObjectBool* o = (ObjectBool*)obj;
		return o->value;
	}else if(t==T::Int){
		ObjectInt* o = (ObjectInt*)obj;
		return o->value!=0;
	}else if(t==T::String){
		ObjectString* o = (ObjectString*)obj;
		return o->value!="";
	}else{
		return true;
	}

#undef T

}
