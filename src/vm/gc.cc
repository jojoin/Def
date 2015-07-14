/**
 * Def 对象分配及垃圾回收
 */

#include <string>
 
#include "gc.h"

using namespace std;

using namespace def::parse;
using namespace def::object;

namespace def {
namespace vm {

/**
 * 构造初始化
 */
Gc::Gc(){
	// 初始化小对象
	prep_nil = new ObjectNil();
	prep_true = new ObjectBool(true);
	prep_false = new ObjectBool(false);
	// 初始化小整数列表
	for(int i=0; i<270; i++){
		prep_ints[i] = new ObjectInt(i-10);
	}
	// 初始化空字符串
	prep_empty_str = new ObjectString("");
}


/**
 * 创建 Int 对象
 */
ObjectBool* Gc::AllotBool(bool val)
{
	return val ? prep_true : prep_false;
}


/**
 * 创建 Int 对象
 */
ObjectInt* Gc::AllotInt(long val)
{
	//cout<<"int = "<<val<<endl;
	if(val<260&&val>=-10){
		// 小整数池 
		//cout<<"get from mini int poll"<<endl;
		return prep_ints[val+10];
	}

	return new ObjectInt(val); 
}



/**
 * 创建 Float 对象
 */
ObjectFloat* Gc::AllotFloat(double val)
{
	//cout<<"float = "<<val<<endl;
	return new ObjectFloat(val);
}


/**
 * 创建 String 对象
 */
ObjectString* Gc::AllotString(string val)
{
	//cout<<"string = "<<val<<endl;
	if(val==""){ //空字符串
		return prep_empty_str;
	}
	return new ObjectString(val);
}



/**
 * 创建 List 对象
 */
ObjectList* Gc::AllotList()
{
	return new ObjectList();
}
/**
 * 创建 dict 对象
 */
ObjectDict* Gc::AllotDict()
{
	return new ObjectDict();
}
/**
 * 创建 block 对象
 */
ObjectBlock* Gc::AllotBlock()
{
	return new ObjectBlock();
}



/**
 * 分配 proc 对象
 */
ObjectProc* Gc::AllotProc(Node*n)
{
	return new ObjectProc( n );
}
/**
 * 分配 func 对象
 */
ObjectFunc* Gc::AllotFunc(Node*n)
{
	return new ObjectFunc( n );
}
/**
 * 分配 node 对象
 */
ObjectNode* Gc::AllotNode(Node*n)
{
	return new ObjectNode( n );
}




/**
 * 从语法节点分配新的对象
 */
DefObject* Gc::Allot(Node* n)
{

#define T NodeType

	T t = n->type;

	if(t==T::Nil){ // nil
		return prep_nil;

	}else if(t==T::Bool){ // bool
		return AllotBool(n->GetBool());

	}else if(t==T::Int){ // int
		return AllotInt(n->GetInt());

	}else if(t==T::Float){ // float
		return AllotFloat(n->GetFloat());

	}else if(t==T::String){ // string
		return AllotString(n->GetString());

	}else if(t==T::List){ return AllotList(); // list
	}else if(t==T::Dict){ return AllotDict(); // dict
	}else if(t==T::Block){ return AllotBlock(); // block
		
	}else if(t==T::ProcDefine){ return AllotProc( n ); // proc
	}else if(t==T::FuncDefine){ return AllotFunc( n ); // func
	// }else if(t==T::Node){ return AllotNode( n );// node
		

	}

#undef T

}


#define T ObjectType

// 容器递归操作
#define CONTAINER_OPT(OPT)\
	if(t==T::List){\
		ObjectList* o = (ObjectList*)obj;\
		size_t len = o->Size();\
		for(size_t i=0; i<len; i++)\
			OPT(o->Visit(i)); \
	}else if(t==T::Block){\
		ObjectBlock* o = (ObjectBlock*)obj;\
		size_t len = o->Size();\
		for(size_t i=0; i<len; i++)\
			OPT(o->Visit(i));\
	}else if(t==T::Dict){\
		map<string, DefObject*> value = ((ObjectDict*)obj)->value;\
		map<string, DefObject*>::iterator iter;\
	    for(iter = value.begin(); iter != value.end(); iter++)\
	        OPT(iter->second);\
	}else if(t==T::Module){\
		map<string, DefObject*> value = ((ObjectModule*)obj)->value;\
		map<string, DefObject*>::iterator iter;\
	    for(iter = value.begin(); iter != value.end(); iter++)\
	        OPT(iter->second);\
	}

// 过滤不需要处理引用的
#define FILTRATE_OPT\
	if(t==T::Nil||t==T::Bool) return true;\
	if(t==T::Int){\
		int val = ((ObjectInt*)obj)->value;\
		if(val<260&&val>=-10) return true; }\
	if( t==T::String && ((ObjectString*)obj)->value=="" )\
		return true;
		
	





/**
 * 引用现有的对象
 * 引用计数 +1
 */
bool Gc::Quote(DefObject* obj)
{
	//cout<<"Gc::Quote"<<endl;
	T t = obj->type;
	// 处理过滤
	FILTRATE_OPT
	// 引用计数 +1
	obj->refcnt += 1;
	// 递归容器加引用
	CONTAINER_OPT(Quote)

	//cout<<"quote refcnt = "<<obj->refcnt<<endl;
	return true;
}


/**
 * 释放对象
 * 引用计数 -1
 * 当引用计数变为0时回收对象，并返回 true 否则返回 false
 * 递归释放容器对象
 */
bool Gc::Free(DefObject* obj)
{
	//cout<<"Gc::Free"<<endl;
	T t = obj->type;
	// 处理过滤
	FILTRATE_OPT
	// 计数 -1
	obj->refcnt -= 1;
	// 递归释放容器对象
	CONTAINER_OPT(Free)
	// 引用归零 回收对象
	if(!obj->refcnt){
		delete obj; // 删除对象
	}
	return true;
}



#undef T
#undef IS_CONTAINER_OBJ
#undef CONTAINER_OPT


} // --end-- namespace vm
} // --end-- namespace def