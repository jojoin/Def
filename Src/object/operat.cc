/** 
 * Def 对象 操作
 */


#include "operat.h"

#include "../util/str.h"



using namespace std;

using namespace def::operat;
using namespace def::object;

using namespace def::util;


#define T ObjectType


/**
 * 取得其 bool 值
 */
bool Conversion::Bool(DefObject *obj)
{

	if(obj==NULL) return false;


	T t = obj->type;

	if(t==T::None){
		return false;
	}else if(t==T::Bool){
		return ((ObjectBool*)obj)->value;
	}else if(t==T::Int){
		return ((ObjectInt*)obj)->value!=0;
	}else if(t==T::String){
		return ((ObjectString*)obj)->value!="";
	}else{
		return true;
	}


}



/**
 * 取得 string 值
 */
string Conversion::String(DefObject *obj)
{

	if(obj==NULL) return "";

	T t = obj->type;

	if(t==T::None){
		return "";
	}else if(t==T::Bool){
		if( ((ObjectBool*)obj)->value ) return "true";
		else return "false";
	}else if(t==T::Int){
		return Str::l2s( ((ObjectInt*)obj)->value ); // long 转 string
	}else if(t==T::String){
		return ((ObjectString*)obj)->value;
	}else{
		return "";
	}

}


/**
 * 取得 long 值
 */
long Conversion::Long(DefObject *obj)
{

	if(obj==NULL) return 0;

	T t = obj->type;

	if(t==T::String){
		return Str::s2l( ((ObjectString*)obj)->value ); // string 转 long
	}else if(t==T::Int){
		return ((ObjectInt*)obj)->value;
	}

	return 0;

}



#undef T
