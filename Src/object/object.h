/** 
 * Def 对象基础
 */

#ifndef DEF_OBJECT_H
#define DEF_OBJECT_H

#include <string>

using namespace std;


namespace def {
namespace object {


// Def 对象类型
enum class ObjType
{
	// 基本
	None,    // none
	Bool,    // 布尔
	Int,     // 整数
	Float,   // 浮点数
	String,  // 字符串

	// 内建容器
	List, // 数组
	Dict, // 字典

	// 高级
	Class  // 类
};


#define T ObjType

/**
 * Def 对象
 */
struct DefObject{
	ObjType type; /*对象类型*/
	DefObject(ObjType t)
		: type(t)
	{}
};


// None none对象
struct ObjectNone : DefObject{
	ObjectNone()
		: DefObject(T::None)
	{}
};


// Int 整型对象
struct ObjectInt : DefObject{
	long value;
	ObjectInt(long v)
		: DefObject(T::Int)
		, value(v)
	{}

};


// String 字符串对象
struct ObjectString : DefObject{
	string value;
	ObjectString(string v)
		: DefObject(T::String)
		, value(v)
	{}
};









#undef T   // ObjType


} // --end-- namespace object
} // --end-- namespace def



#endif
// --end-- DEF_OBJECT_H








