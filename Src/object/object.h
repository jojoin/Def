#ifndef DEF_OBJECT_OBJECT_H
#define DEF_OBJECT_OBJECT_H

/** 
 * Def 对象基础
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../parse/node.h"
// #include "../vm/stack.h"

using namespace std;

using namespace def::parse;
// using namespace def::vm;


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
	List,  // 列表
	Dict,  // 字典
	Block, // 块

	// 节点
	Func,  // 函数
	Proc,  // 处理器
	Node,  // 解析树节点

	// 高级
	Class,   // 类
	Object,  // 对象实例

	Module   // 模块

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
	static void Print(DefObject*); // 打印
};



// None def 处理器对象
struct ObjectProc : DefObject{
	Node* value; //指向对应语法节点
	ObjectProc()
		: DefObject(T::Proc)
	{}
};

// None defun 函数对象
struct ObjectFunc : DefObject{
	Node* value; //指向对应语法节点
	ObjectFunc()
		: DefObject(T::Func)
	{}
};

// None node 语句节点对象
struct ObjectNode : DefObject{
	Node* value; //指向对应语法节点
	ObjectNode()
		: DefObject(T::Node)
	{}
};




// *************************** //




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


// Float 浮点对象
struct ObjectFloat : DefObject{
	double value;
	ObjectFloat(double v)
		: DefObject(T::Float)
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



// 顺序结构对象父类
struct ObjectExArr : DefObject{
	vector<DefObject*> value;
	ObjectExArr(T t)
		: DefObject(t)
	{
		value = vector<DefObject*>();
	}
	// 列表末尾添加对象
	DefObject* Push(DefObject* obj){
        // cout<<"list push"<<endl;
		value.push_back(obj);
		return obj;
	}
	// 替换
	DefObject* Push(size_t i, DefObject* obj){
        // cout<<"list replace"<<endl;
		value[i] = obj;
		return obj;
	}
	// 返回列表大小
	size_t Size(){
        // cout<<"list size"<<endl;
        return value.size();
	}
	// 访问元素
	DefObject* Visit(size_t i){
		if( i>=0 && i<value.size() ){
			return value[i];
		}else{
			return NULL;
		}
	}

};


// List 列表对象
struct ObjectList : ObjectExArr{
	ObjectList()
	: ObjectExArr(T::List)
	{}

};



// Block 块对象
struct ObjectBlock : ObjectExArr{
	ObjectBlock()
		: ObjectExArr(T::Block)
	{}
};





// 字典型对象父类
struct ObjectExPkg : DefObject{
	map<string, DefObject*> value;
	ObjectExPkg(T t)
		: DefObject(t)
	{}

	// 替换设置元素
	DefObject* Replace(string key,  DefObject* obj){
        // cout<<"dict push"<<endl;
        // value.insert(map<string, DefObject*>::value_type(key, obj));
        value[key] = obj;
		return obj;
	}
	// 添加元素
	DefObject* Insert(string key,  DefObject* obj){
        // cout<<"dict push"<<endl;
        value.insert(map<string, DefObject*>::value_type(key, obj));
		return obj;
	}

	// 返回列表大小
	size_t Size(){
        // cout<<"dict size"<<endl;
        // return value.size();
        return 0;
	}
	// 访问元素
	DefObject* Visit(string key){
        // cout<<"dict visit"<<endl;
        if(key==""){
        	return NULL;
        }
        map<string, DefObject*>::iterator iter = value.find(key);
		if(iter!=value.end())
		{
		    return iter->second;
		}
		return NULL;
	}

};



// Dict 字典对象
struct ObjectDict : ObjectExPkg{
	ObjectDict()
		: ObjectExPkg(T::Dict)
	{}
};


// Class 类对象
struct ObjectClass : ObjectExPkg{
	ObjectClass()
		: ObjectExPkg(T::Class)
	{}
};


// Object 对象实例
struct ObjectObject : ObjectExPkg{
	ObjectObject()
		: ObjectExPkg(T::Object)
	{}
};


// Module 模块对象
struct ObjectModule : ObjectExPkg{
	string file; // 模块文件绝对路径
	ObjectModule()
		: ObjectExPkg(T::Module)
	{}
};




#undef T   // ObjectType

















} // --end-- namespace object
} // --end-- namespace def



#endif
// --end-- DEF_OBJECT_OBJECT_H








