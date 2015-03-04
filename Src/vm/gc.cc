/**
 * Def 对象分配及垃圾回收
 */

#include <string>
 
#include "gc.h"

using namespace std;

using namespace def::object;
using namespace def::node;
using namespace def::gc;

/**
 * 构造初始化
 */
Gc::Gc(){
	// 初始化小对象
	prep_none = new ObjectNone();
	prep_true = new ObjectBool(true);
	prep_false = new ObjectBool(false);
	// 初始化小整数列表
	for(int i=0; i<270; i++){
		prep_ints[i] = new ObjectInt(i-10);
	}
	// 初始化小字符串列表
	// TODO::
	
}


/**
 * 从语法节点分配新的对象
 */
DefObject* Gc::Allot(Node* n){

#define T NodeType

	T t = n->type;

	if(t==T::None){ // none
		return prep_none;
	}else if(t==T::Bool){ // bool
		if(n->GetBool())
			return prep_true;
		else
			return prep_false;
	}else if(t==T::Int){ // int
		long val = n->GetInt();
		if(val>=-10&&val<=260){
			// 小整数池
			cout<<"mini int poll"<<endl;
			return prep_ints[val+10];
		}
		return new ObjectInt(val);
	}else if(t==T::String){ // string

	}

#undef T

}


/**
 * 引用现有的对象
 * 引用计数 +1
 */
DefObject* Gc::Quote(DefObject* obj){
	// 引用计数 +1
	obj->refcnt += 1;
	cout<<"Gc::Quote"<<endl;
}



#define T ObjectType
#define IS_CONTAINER_OBJ obj->type==T::List||obj->type==T::Dict


/**
 * 释放对象
 * 引用计数 -1
 * 当引用计数变为0时回收对象
 * 递归释放容器对象
 */
DefObject* Gc::Free(DefObject* obj){
	T t = obj->type;
	size_t r = obj->refcnt;
	// 递归释放容器对象
	if(IS_CONTAINER_OBJ){
	   	// TODO:: 
	}
	// 引用计数 -1
	obj->refcnt = r--;
	// 引用归零 回收对象
	if(r<=0){
		Recycle(obj);
	}
}


/**
 * 回收对象
 * 递归容器对象 判断是否回收
 */
bool Gc::Recycle(DefObject* obj){
	T t = obj->type;
	if(t==T::None||t==T::Bool){
		return true; // 对象池不需要 del
	}else if(t==T::Int){
		ObjectInt* obj_int = (ObjectInt*)obj;
		long val = obj_int->value;
		if(val>=-10&&val<=260){
			// 小整数池 不需要 del
		}
	}

	// delete 对象指针
	delete obj;
	return true;
}


#undef T
#undef IS_CONTAINER_OBJ
