
#ifndef DEF_NODE_H
#define DEF_NODE_H

#include <string>
#include <vector>

using namespace std;
//using namespace def::util;


namespace def {
namespace node {

// 节点类型
enum class TypeNode
{
	Expression,           // 组合表达式

	Assignment,           // 赋值语句

	ArithAdd,             // + 加法计算
	ArithSub,             // - 减法计算
	ArithMul,             // * 乘法计算
	ArithDiv,             // / 除法计算

	FunctionDefine,       // 函数定义
	FunctionCall,         // 函数调用

	// 叶节点
	Variable,             // 变量符号

	ValueNone,            // 值 none
	ValueBool,            // 值 布尔
	ValueInt,             // 值 整数
	ValueFloat,           // 值 浮点数
	ValueString,          // 值 字符串

	
}; // --end-- enum class TypeNode


// 节点
struct Node{
	unsigned int line;   // 代码行
	unsigned int posi;   // 所属位置
	TypeNode type;           // 节点类型
	// 构造方法
	Node(TypeNode t=TypeNode::Expression, unsigned int l=0, unsigned int p=0)
		: type(t), line(l), posi(p){}
};

// 多叉节点
struct NodeTree : Node{
	vector<Node*> childs; // 子节点指针列表
	NodeTree(TypeNode t=TypeNode::Expression, unsigned int l=0, unsigned int p=0)
		: Node(t, l, p){}
	void AddChild(Node* n){
		childs.push_back(n);
	}
};

// 二叉节点
struct NodeTwinTree : Node{
	Node* left;   // 左子节点
	Node* right;  // 右子节点
	NodeTwinTree(TypeNode t, unsigned int l=0, unsigned int p=0, Node* lf=NULL, Node* rt=NULL)
		: Node(t, l, p), left(lf), right(rt){}
};

// variable 节点
struct NodeVariable : Node{
	string name;
	NodeVariable(unsigned int l, unsigned int p, string n)
		: Node(TypeNode::Variable, l, p), name(n){}
};

// none 节点
struct NodeNone : Node{
	NodeNone(unsigned int l, unsigned int p)
		: Node(TypeNode::ValueNone, l, p){}
};

// bool 节点
struct NodeBool : Node{
	bool value;
	NodeBool(unsigned int l, unsigned int p, bool v)
		: Node(TypeNode::ValueBool, l, p), value(v){}
};

// int 节点
struct NoneInt : Node{
	long value;
	NoneInt(unsigned int l, unsigned int p, long v)
		: Node(TypeNode::ValueBool, l, p), value(v){}
};







} // --end-- namespace node
} // --end-- namespace def



#endif
// --end-- DEF_NODE_H

