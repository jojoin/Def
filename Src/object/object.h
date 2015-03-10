#ifndef DEF_OBJECT_H
#define DEF_OBJECT_H

/** 
 * Def 对象基础
 */

#include <string>

using namespace std;


namespace def {
namespace object {


// Def 对象类型
enum class ObjectType
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


#define T ObjectType

/**
 * Def 对象
 */
struct DefObject{
	T type;  /*对象类型*/
	size_t refcnt; /*引用计数*/
	DefObject(T t, size_t r=1)
		: type(t)
		, refcnt(r)
	{}
};


// None none 对象
struct ObjectNone : DefObject{
	ObjectNone()
		: DefObject(T::None)
	{}
};


// Bool true 对象
struct ObjectBool : DefObject{
	bool value;
	ObjectBool(bool v)
		: DefObject(T::Bool)
		, value(v)
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









#undef T   // ObjectType


} // --end-- namespace object
} // --end-- namespace def



#endif
// --end-- DEF_OBJECT_H








