#ifndef DEF_OBJECT_H
#define DEF_OBJECT_H

/** 
 * Def 对象基础
 */

#include <iostream>
#include <string>
#include <vector>

using namespace std;


namespace def {
namespace object {


// Def 对象类型
enum class ObjectType
{
	// 系统
	Error,   // 执行错误

	// 基本
	None,    // none
	Bool,    // 布尔
	Int,     // 整数
	Float,   // 浮点数
	String,  // 字符串

	// 容器
	Tuple, // 元组
	List,  // 数组
	Dict,  // 字典

	// 节点
	Func,  // 函数
	Proc,  // 处理器
	Node,  // 解析树节点

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
	DefObject(T t, size_t r=0)
		: type(t)
		, refcnt(r)
	{}
	// virtual DefObject* Push(DefObject*){};
	// virtual DefObject* Visit(size_t){};
	// virtual size_t Size(){};
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


// List 列表对象
struct ObjectList : DefObject{
	vector<DefObject*> value;
	ObjectList()
		: DefObject(T::List)
	{
		value.clear();
	}
	// 列表末尾添加对象
	DefObject* Push(DefObject* obj){
        // cout<<"list push"<<endl;
		value.push_back(obj);
		return obj;
	}
	// 返回列表大小
	size_t Size(){
        // cout<<"list size"<<endl;
        // cout<<value.size()<<endl;
        return value.size();
	}
	// 访问元素
	DefObject* Visit(size_t i){
        // cout<<"list isit"<<endl;
		return value[i];
	}

};









#undef T   // ObjectType


} // --end-- namespace object
} // --end-- namespace def



#endif
// --end-- DEF_OBJECT_H








