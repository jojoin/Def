
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
	Normal,               // 默认状态

	Expression,           // 组合表达式

	Assignment,           // 赋值语句

	Add,             // + 加法计算
	Sub,             // - 减法计算
	Mul,             // * 乘法计算
	Div,             // / 除法计算

	FunctionDefine,       // 函数定义
	FunctionCall,         // 函数调用

	// 叶节点
	Variable,             // 变量符号

	None,            // 值 none
	Bool,            // 值 布尔
	Int,             // 值 整数
	Float,           // 值 浮点数
	String,          // 值 字符串

	
}; // --end-- enum class TypeNode


// 节点
struct Node{
	unsigned int line;   // 代码行
	unsigned int posi;   // 所属位置
	TypeNode type;           // 节点类型
	// 构造方法
	Node(TypeNode t, unsigned int l=0, unsigned int p=0)
		: type(t), line(l), posi(p){}
	virtual ~Node()=0;
};
Node::~Node(){} // 纯虚析构函数的定义


// 多叉节点
struct NodeTree : Node{
	vector<Node*> childs; // 子节点指针列表
	NodeTree(TypeNode t, unsigned int l=0, unsigned int p=0)
		: Node(t, l, p){}
	virtual ~NodeTree()=0;
	void AddChild(Node* n){
        cout<<"add child "<<(int)n->type<<endl;
		childs.push_back(n);
	}
};
NodeTree::~NodeTree(){} // 纯虚析构函数的定义


// 组合表达式
struct NodeExpression : NodeTree{
	NodeExpression(unsigned int l=0, unsigned int p=0)
		: NodeTree(TypeNode::Expression, l, p){}
	~NodeExpression(){
		size_t sz = childs.size();
		for(int i=0; i<sz; i++)
		{
        	cout<<"delete child "<<(int)childs[i]->type<<endl;
			delete childs[i];
		}
		childs.clear();
	}
};


// 二叉节点
struct NodeTwinTree : Node{
	Node* left;   // 左子节点
	Node* right;  // 右子节点
	NodeTwinTree(TypeNode t, unsigned int l=0, unsigned int p=0, Node* lf=NULL, Node* rt=NULL)
		: Node(t, l, p), left(lf), right(rt){}
	virtual ~NodeTwinTree()=0;
	void LeftChild(Node* n){
        cout<<"left child "<<(int)n->type<<endl;
		left = n;
	}
	void RightChild(Node* n){
        cout<<"right child "<<(int)n->type<<endl;
		right = n;
	}
};
NodeTwinTree::~NodeTwinTree(){} // 纯虚析构函数的定义


// = 赋值节点
struct NodeAssignment: NodeTwinTree{
	NodeAssignment(unsigned int l=0, unsigned int p=0)
		: NodeTwinTree(TypeNode::Assignment, l, p){}
	~NodeAssignment(){
        cout<<"delete left "<<(int)left->type<<endl;
		delete left;
        cout<<"delete right "<<(int)right->type<<endl;
		delete right;
	}
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
		: Node(TypeNode::None, l, p){}
};


// bool 节点
struct NodeBool : Node{
	bool value;
	NodeBool(unsigned int l, unsigned int p, bool v)
		: Node(TypeNode::Bool, l, p), value(v){}
};


// int 节点
struct NodeInt : Node{
	long value;
	NodeInt(unsigned int l, unsigned int p, long v)
		: Node(TypeNode::Int, l, p), value(v){}
};








} // --end-- namespace node
} // --end-- namespace def



#endif
// --end-- DEF_NODE_H

