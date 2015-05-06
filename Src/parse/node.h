#ifndef DEF_NODE_H
#define DEF_NODE_H

#include <string>
#include <vector>

#include "token.h"
#include "tokenizer.h"

#include "../Util/str.h"

using namespace std;
using namespace def::util;


namespace def {
namespace node {

// 节点类型
enum class NodeType
{
	Normal,   // 默认状态

	// 枝节点

	Group,    // 组合表达式

	Assign,   // 赋值语句

	Add,   // + 加法计算              // 5
	Sub,   // - 减法计算
	Mul,   // * 乘法计算
	Div,   // / 除法计算

	FuncDefine,   // 函数定义
	FuncCall,     // 函数调用

	Print,   // 打印

	List,   // 列表
	Dict,   // 字典

	// 叶节点

	Variable,   // 变量符号           // 11

	None,    // 值 none
	Bool,    // 值 布尔
	Int,     // 值 整数
	Float,   // 值 浮点数
	String,  // 值 字符串

	Priority,  // () 括号优先级

	// 控制流结构

	If,  // if elif else
	While,  // while 循环

	End,   // 终止并返回
	
}; // --end-- enum class NodeType



#define NT NodeType



// 节点
struct Node{
	size_t line;   // 代码行
	size_t posi;   // 所属位置
	NT type;       // 节点类型
	// 构造方法
	Node(NT t, Word &w)
		: type(t)
	{
		line = w.line;
		posi = w.posi;
	}
	//判断是否为变量的字面量
	inline bool IsValue(){
	    if(type==NT::Variable){
	        return false;
	    }else if(
	        type==NT::None ||
	        type==NT::Bool ||
	        type==NT::Int ||
	        type==NT::Float ||
	        type==NT::String
	    ){
	        return true;
	    }
	    return false;
	}
	//判断是否为运算操作节点
	inline bool IsOperate(){
	    if(
	        type==NT::Add ||
	        type==NT::Sub ||
	        type==NT::Mul ||
	        type==NT::Div
	    ){
	        return true;
	    }
	    return false;
	}

	virtual ~Node()=0;
	virtual void AddChild(Node*){};
	virtual void ClearChild(){};
	virtual Node* Child(size_t i){};
	virtual size_t ChildSize(){};
	virtual Node* Left(Node*n=NULL){};
	virtual Node* Right(Node*n=NULL){};
	virtual bool GetBool(){};
	virtual long GetInt(){};
	virtual double GetFloat(){};
	virtual string GetName(){};
	virtual string GetString(){};
};


// 多叉节点
struct NodeTree : Node{
	vector<Node*> childs; // 子节点指针列表
	NodeTree(NT t, Word &w)
		: Node(t, w){}
	virtual ~NodeTree(){
		size_t s = childs.size();
		for(size_t i=0; i<s; i++)
		{
        	//cout<<"delete child "<<i<<endl;
			delete childs[i];
		}
		childs.clear();
	};
	inline void AddChild(Node* n){
        //cout<<"add child "<<(int)n->type<<endl;
		childs.push_back(n);
	}
	inline void ClearChild(){
		childs.clear();
	};
	inline Node* Child(size_t i){
		return i<childs.size() ? childs[i] : NULL;
	}
	inline size_t ChildSize(){ //子节点数量
		return childs.size();
	};
	inline Node* ChildPop(size_t i=0){ //弹出某个子节点
		Node* n = Child(i);
		if(n){
			childs.erase(childs.begin()+i);
		}
		return n;
	};
};


// 组合表达式
struct NodeGroup : NodeTree{
	NodeGroup(Word &w)
		: NodeTree(NT::Group, w){}
};


// def 函数定义结构
struct NodeFuncDefine : Node{
	NodeGroup* body; //函数体
	NodeFuncDefine(Word &w)
		: Node(NT::FuncDefine, w){}
};


// def 函数调用结构
struct NodeFuncCall : Node{
	NodeFuncCall(Word &w)
		: Node(NT::FuncCall, w){}
};


// [] 列表数据结构
struct NodeList : NodeTree{
	NodeList(Word &w)
		: NodeTree(NT::List, w){}
};




// If 控制流程
struct NodeIf : NodeTree{
	NodeIf(Word &w)
		: NodeTree(NT::If, w){}
};


// 二叉节点
struct NodeTwinTree : Node{
	Node* left;   // 左子节点
	Node* right;  // 右子节点
	NodeTwinTree(NT t, Word &w, Node* lf=NULL, Node* rt=NULL)
		: Node(t, w), left(lf), right(rt){}
	virtual ~NodeTwinTree(){
        //cout<<"delete left "<<(int)left->type<<endl;
		delete left;
        //cout<<"delete right "<<(int)right->type<<endl;
		delete right;
	};
	inline Node* Left(Node*n=NULL){
        //cout<<"left child "<<(int)n->type<<endl;
        if(n!=NULL){
			left = n; //设置
        }
        return left;
	}
	inline Node* Right(Node*n=NULL){
        //cout<<"right child "<<(int)n->type<<endl;
        if(n!=NULL){
			right = n; //设置
        }
        return right;
	}
};


// while 循环控制
struct NodeWhile : NodeTwinTree{
	NodeWhile(Word &w)
		: NodeTwinTree(NT::While, w){}
};


/*
// end 执行并返回
struct NodeEnd : NodeTwinTree{
	NodeEnd(Word &w)
		: NodeTwinTree(NT::Return, w){}
};
*/

// = 赋值节点
struct NodeAssign: NodeTwinTree{
	NodeAssign(Word &w)
		: NodeTwinTree(NT::Assign, w){}
};


// + 加操作节点
struct NodeAdd: NodeTwinTree{
	NodeAdd(Word &w)
	: NodeTwinTree(NT::Add, w){}
};


// - 减操作节点
struct NodeSub: NodeTwinTree{
	NodeSub(Word &w)
	: NodeTwinTree(NT::Sub, w){}
};

// * 乘操作节点
struct NodeMul: NodeTwinTree{
	NodeMul(Word &w)
	: NodeTwinTree(NT::Mul, w){}
};

// / 除操作节点
struct NodeDiv: NodeTwinTree{
	NodeDiv(Word &w)
	: NodeTwinTree(NT::Div, w){}
};


// print 打印变量至屏幕
struct NodePrint : NodeTwinTree{
	NodePrint(Word &w)
	: NodeTwinTree(NT::Print, w)
	{}
};


// variable 节点
struct NodeVariable : Node{
	string name;
	NodeVariable(Word &w)
	: Node(NT::Variable, w){
		name = w.value;
	}
	inline string GetName(){
		return name;
	};
};


// none 节点
struct NodeNone : Node{
	NodeNone(Word &w)
	: Node(NT::None, w){}
};


// bool 节点
struct NodeBool : Node{
	bool value;
	NodeBool(Word &w)
	: Node(NT::Bool, w){
		value = w.value=="true" ? true : false;
	}
	inline bool GetBool(){
		return value;
	};
};


// int 节点
struct NodeInt : Node{
	long value;
	NodeInt(Word &w)
	: Node(NT::Int, w){
		value = Str::s2l(w.value);
	}
	inline long GetInt(){
		return value;
	};
};


// float 节点
struct NodeFloat : Node{
	double value;
	NodeFloat(Word &w)
	: Node(NT::Float, w){
		value = Str::s2d(w.value);
	}
	inline double GetFloat(){
		return value;
	};
};


// string 节点
struct NodeString : Node{
	string value;
	NodeString(Word &w)
	: Node(NT::String, w){
		value = w.value;
	}
	inline string GetString(){
		return value;
	};
};

#undef NT   // NodeType





} // --end-- namespace node
} // --end-- namespace def



#endif
// --end-- DEF_NODE_H

