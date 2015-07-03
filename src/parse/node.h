#ifndef DEF_VM_NODE_H
#define DEF_VM_NODE_H

#include <string>
#include <vector>

#include "token.h"
#include "tokenizer.h"

#include "../util/str.h"

using namespace std;
using namespace def::util;


namespace def {
namespace parse {

#define NODELIST(N, D)              \
								\
	N(Normal, 0)              	\
	D(Group, 0)              	\
								\
	D(Variable, 0)				\
	D(None, 0)					\
	D(Bool, 0)					\
	D(Int, 0)					\
	D(Float, 0)					\
	D(String, 0)				\
								\
	D(Not, 0)					\
								\
	D(Assign, 1)              	\
	D(AssignUp, 1)             	\
								\
	D(Equal, 3)					\
	D(More, 3)					\
	D(Less, 3)					\
	D(MoreEqual, 3)				\
	D(LessEqual, 3)				\
	D(NotEqual, 3)				\
								\
	D(Add, 6)					\
	D(Sub, 6)					\
								\
	D(Mul, 7)					\
	D(Div, 7)					\
								\
	D(FuncCall, 9)				\
	D(ProcCall, 9)				\
	D(ContainerAccess, 10)		\
	D(MemberAccess, 10)		    \
								\
	D(List, 0)					\
	D(Dict, 0)					\
	D(Block, 0)					\
								\
	D(If, 0)					\
	D(While, 0)              	\
	D(For, 0)              		\
								\
	D(FuncDefine, 0)			\
	D(ProcDefine, 0)			\
	D(ClassDefine, 0)			\
								\
	D(Import, 0)              	\
	D(Priority, 0)				\
	D(Print, 0)              	\
	D(Return, 0)              	\
								\
	N(End, 0)            


	// 符号与关键字具体值
#define N(name, priority) name,
#define D(name, priority) name,
	enum class NodeType {
		NODELIST(N, D)
	};
#undef N
#undef D




#define NT NodeType

// 打印节点缩进
#define PRT "  "

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

	virtual void AddChild(Node*n=NULL, bool f=false){};
	virtual Node* Child(size_t i=0){};
	virtual size_t ChildSize(){};
	virtual void ClearChild(){};

	virtual Node* Left(Node*n=NULL){};
	virtual Node* Right(Node*n=NULL){};

	virtual bool GetBool(){};
	virtual long GetInt(){};
	virtual void SetInt(long n=0){};
	virtual double GetFloat(){};

	virtual string GetName(){};
	virtual void SetName(string n=""){};

	virtual string GetString(){};
	virtual void SetString(string n=""){};

	virtual void Print(string prefix=""){};
	virtual bool IsTwinTree(){};

	virtual void SetArgv(Node*g=NULL){};
	virtual Node* GetArgv(){};

	virtual void SetBody(Node*g=NULL){};
	virtual Node* GetBody(){};
};


// 单叉节点
struct NodeOneTree : Node{
	Node* child;   // 子节点
	NodeOneTree(NT t, Word &w, Node* ch=NULL)
	: Node(t, w), child(ch){}
	virtual ~NodeOneTree(){
        //cout<<"delete child "<<(int)child->type<<endl;
		delete child;
	}
	inline Node* Child(size_t i=0){
        return child;
	}
	inline void AddChild(Node*n, bool f=false){
        // cout << "NodeOneTree AddChild, n = " << (int)n << endl;
		child = n; //设置
        // cout << "child = " << (int)child << endl;

	}
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
	}
	inline bool IsTwinTree(){
		return true;
	}
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
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"NodeTwinTree: "<<endl;
		if(left) left->Print(prefix+PRT);
		if(right) right->Print(prefix+PRT);
	};
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
	}
	inline void AddChild(Node* n, bool f=false){
        //cout<<"add child "<<(int)n->type<<endl;
        if(f) childs.insert(childs.begin(), n);
        else childs.push_back(n);
		
	}
	inline void ClearChild(){
		childs.clear();
	}
	inline Node* Child(size_t i){
		return i<childs.size() ? childs[i] : NULL;
	}
	inline size_t ChildSize(){ //子节点数量
		return childs.size();
	}
	inline Node* ChildPop(size_t i=0){ //弹出某个子节点
		Node* n = Child(i);
		if(n){
			childs.erase(childs.begin()+i);
		}
		return n;
	}
	inline void Print(string prefix=""){ // 打印
		size_t sz = ChildSize();
		cout<< prefix+"NodeTree: "<<sz<<endl;
		for(size_t i=0; i<sz; i++)
		{
			Child(i)->Print(prefix+PRT);
		}
	};
};



// 单叉节点
#define NODEONETREE(xxx)                      \
struct Node##xxx : NodeOneTree{               \
	Node##xxx(Word &w, Node*ch=NULL)          \
	: NodeOneTree(NT::xxx, w, ch){}           \
	inline void Print(string prefix=""){      \
		cout<<prefix+#xxx+":"<<endl;          \
		if(child) child->Print(prefix+PRT);   \
	};                                        \
};


// 双叉节点
#define NODETWINTREE(xxx)                    \
struct Node##xxx : NodeTwinTree{             \
	Node##xxx(Word &w)                       \
	: NodeTwinTree(NT::xxx, w){}             \
	inline void Print(string prefix=""){     \
		cout<<prefix+#xxx+":"<<endl;         \
		if(left) left->Print(prefix+PRT);    \
		if(right) right->Print(prefix+PRT);  \
	};                                       \
};


// 多叉节点
#define NODETREE(xxx)                        \
struct Node##xxx : NodeTree{                 \
	Node##xxx(Word &w)                       \
	: NodeTree(NT::xxx, w){}                 \
	inline void Print(string prefix=""){     \
		size_t sz = ChildSize();             \
		cout<<prefix+#xxx+":"<<sz<<endl;     \
		for(size_t i=0; i<sz; i++){          \
			Node *cd = Child(i);             \
			if(cd) cd->Print(prefix+PRT);    \
			else cout<<prefix+PRT+"*null*"<<endl; /*可能为空 占位符*/ \
		}                                    \
	};                                       \
};




// 单叉子节点
NODEONETREE(Print)      // print 打印变量至屏幕
NODEONETREE(Priority)   // Priority 优先级
NODEONETREE(Import)     // Import 模块加载
NODEONETREE(Return)     // Return 函数返回
NODEONETREE(Not)            // ~


// 双叉子节点
NODETWINTREE(Assign)             // : 赋值节点
NODETWINTREE(AssignUp)           // :: 向上查找赋值节点
NODETWINTREE(ProcCall)           // 处理器调用
NODETWINTREE(FuncCall)           // 函数调用
NODETWINTREE(ContainerAccess)    // 列表和字典 容器访问
NODETWINTREE(MemberAccess)       // 类或模块成员访问
NODETWINTREE(Add)        // + 加操作节点
NODETWINTREE(Sub)        // - 减操作节点
NODETWINTREE(Mul)        // * 乘操作节点
NODETWINTREE(Div)        // / 除操作节点
//条件判断
NODETWINTREE(Equal)          // = 
NODETWINTREE(More)           // >
NODETWINTREE(Less)           // <
NODETWINTREE(MoreEqual)      // >=
NODETWINTREE(LessEqual)      // <=
NODETWINTREE(NotEqual)       // ~=


// 多叉子节点
NODETREE(Group)    // Group 组合表达式
NODETREE(If)       // If 条件判断
NODETREE(While)    // While 循环控制
NODETREE(For)      // For 遍历
NODETREE(List)     // () 列表数据结构
NODETREE(Dict)     // [] 字典数据结构
NODETREE(Block)    // {} 块结构




// def defun 处理器和函数公用定义结构
struct NodeExDefine : Node{
	string name; // 名称，可匿名
	Node* argv;  // 参数列表
	Node* body;  // 体
	NodeExDefine(NT t, Word &w, string n="")
	: Node(t, w)
	, name(n)
	{
		argv = NULL;
		body = NULL;
	}
	virtual ~NodeExDefine(){
		delete argv;
		delete body;
	}
	inline string GetName(){ return name; }
	inline void SetName(string n=""){ name = n; }
	inline Node* GetArgv(){ return argv; }
	inline void SetArgv(Node*g=NULL){ argv = g; }
	inline Node* GetBody(){ return body; }
	inline void SetBody(Node*g=NULL){ body = g; }
	inline void Print(string prefix=""){ // 打印
		string head = "";
		switch(type){
			case NT::ProcDefine: head = "ProcDefine"; break;
			case NT::FuncDefine: head = "FuncDefine"; break;
			case NT::ClassDefine: head = "ClassDefine"; break;
		}
		cout<<prefix+head+": "<<name<<endl;
		prefix += PRT;
		cout<<prefix<<"argv: "<<endl;
		if(argv) argv->Print(prefix);
		cout<<prefix+"body: "<<endl;
		if(body) body->Print(prefix);
	};
};


// 公用定义结构
#define NODEEXDEFINE(xxx)                    \
struct Node##xxx : NodeExDefine{             \
	Node##xxx(Word &w)                       \
	: NodeExDefine(NT::xxx, w){}             \
};

NODEEXDEFINE(ProcDefine)        // 处理器定义
NODEEXDEFINE(FuncDefine)        // 函数定义
NODEEXDEFINE(ClassDefine)       // 类定义

#undef NODEEXDEFINE




//*************  数据节点（叶节点）  **************//



// variable 节点
struct NodeVariable : Node{
	string name;
	NodeVariable(Word &w)
	: Node(NT::Variable, w)
	, name(w.value){

	}
	inline string GetName(){
		return name;
	};
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"Variable: $"<<name<<endl;
	};
};


// none 节点
struct NodeNone : Node{
	NodeNone(Word &w)
		: Node(NT::None, w){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"None"<<endl;
	};
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
	}
	inline void Print(string prefix=""){ // 打印
		string bl = value ? "true" : "false";
		cout<<prefix+"Bool: "<<bl<<endl;
	};
};


// int 节点
struct NodeInt : Node{
	long value;
	NodeInt(Word &w)
	: Node(NT::Int, w){
		value = Str::s2l(w.value);
	}
	inline void SetInt(long n){
		value = n;
	}
	inline long GetInt(){
		return value;
	}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"Int: "<<value<<endl;
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
	}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"Float: "<<value<<endl;
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
	}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"String: \""<<value<<"\""<<endl;
	};
};









#undef NT   // NodeType
#undef PRT   // 打印缩进





} // --end-- namespace parse
} // --end-- namespace def



#endif
// --end-- DEF_VM_NODE_H

