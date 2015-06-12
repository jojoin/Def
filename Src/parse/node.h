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
	D(Assign, 1)              	\
	D(AssignUp, 1)             	\
								\
	D(Add, 2)					\
	D(Sub, 2)					\
								\
	D(Mul, 3)					\
	D(Div, 3)					\
								\
								\
	D(If, 0)					\
	D(While, 0)              	\
								\
	D(FuncCall, 4)				\
	D(ProcCall, 4)				\
	D(ContainerAccess, 4)		\
	D(MemberAccess, 5)		    \
								\
	D(List, 0)					\
	D(Dict, 0)					\
	D(Block, 0)					\
								\
	D(FuncDefine, 0)			\
	D(ProcDefine, 0)			\
	D(ClassDefine, 0)			\
								\
	D(Priority, 0)				\
								\
	D(Print, 0)              	\
								\
	D(Import, 0)              	\
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
		Left()->Print(prefix+PRT);
		Right()->Print(prefix+PRT);
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


// 组合表达式
struct NodeGroup : NodeTree{
	NodeGroup(Word &w)
	: NodeTree(NT::Group, w){}
	inline void Print(string prefix=""){ // 打印
		size_t sz = ChildSize();
		cout<< prefix+"Group: "<<sz<<endl;
		for(size_t i=0; i<sz; i++)
		{
			Node*cd = Child(i);
			if(cd) cd->Print(prefix+PRT);
			else cout<<prefix+PRT+"*null*"<<endl; //可能为空 占位符
		}
	};
};


// 优先级
struct NodePriority : NodeOneTree{
	NodePriority(Word &w, Node*ch=NULL)
	: NodeOneTree(NT::Priority, w, ch){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"Priority:"<<endl;
		if(child) child->Print(prefix);
		// Child()->Print(prefix);
	};
};


// 模块加载
struct NodeImport : NodeOneTree{
	NodeImport(Word &w, Node*ch=NULL)
	: NodeOneTree(NT::Import, w, ch){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"Import:"<<endl;
		if(child) child->Print(prefix);
	};
};



// def defun 处理器和函数公用定义结构
struct NodeExDefine : Node{
	string name; //处理器名称，可匿名
	Node* argv; //处理器参数列表
	Node* body; //处理器体
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

// def 处理器定义结构
struct NodeProcDefine : NodeExDefine{
	NodeProcDefine(Word &w)
	: NodeExDefine(NT::ProcDefine, w)
	{}
};

// defun 函数定义结构
struct NodeFuncDefine : NodeExDefine{
	NodeFuncDefine(Word &w)
	: NodeExDefine(NT::FuncDefine, w)
	{}
};

// class 类定义结构
struct NodeClassDefine : NodeExDefine{
	NodeClassDefine(Word &w)
	: NodeExDefine(NT::ClassDefine, w)
	{}
};






// def 函数调用结构
struct NodeProcCall : NodeTwinTree{
	NodeProcCall(Word &w)
	: NodeTwinTree(NT::ProcCall, w)
	{}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"ProcCall:"<<endl;
		Left()->Print(prefix+PRT);
		Right()->Print(prefix+PRT);
	};

};


// def 函数调用结构
struct NodeFuncCall : NodeTwinTree{
	NodeFuncCall(Word &w)
	: NodeTwinTree(NT::FuncCall, w)
	{}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"FuncCall:"<<endl;
		Left()->Print(prefix+PRT);
		Right()->Print(prefix+PRT);
	};
};


// 列表和字典 容器访问结构
struct NodeContainerAccess : NodeTwinTree{
	NodeContainerAccess(Word &w)
	: NodeTwinTree(NT::ContainerAccess, w){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"ContainerAccess:"<<endl;
		Left()->Print(prefix+PRT);
		Right()->Print(prefix+PRT);
	};
};


// 类或模块成员访问
struct NodeMemberAccess : NodeTwinTree{
	NodeMemberAccess(Word &w)
	: NodeTwinTree(NT::MemberAccess, w){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"MemberAccess:"<<endl;
		Left()->Print(prefix+PRT);
		Right()->Print(prefix+PRT);
	};
};





// while 循环控制
struct NodeWhile : NodeTree{
	NodeWhile(Word &w)
	: NodeTree(NT::While, w){}
	inline void Print(string prefix=""){ // 打印
		size_t sz = ChildSize();
		cout<< prefix+"While: size="<<sz<<endl;
		for(size_t i=0; i<sz; i++)
		{
			Child(i)->Print(prefix+PRT);
		}
	};
};


// If 控制流程
struct NodeIf : NodeTree{
	NodeIf(Word &w)
	: NodeTree(NT::If, w){}
	inline void Print(string prefix=""){ // 打印
		size_t sz = ChildSize();
		cout<< prefix+"If: branch="<<sz<<endl;
		for(size_t i=0; i<sz; i++)
		{
			Child(i)->Print(prefix+PRT);
		}
	};
};



// () 列表数据结构
struct NodeList : NodeTree{
	NodeList(Word &w)
	: NodeTree(NT::List, w){}
	inline void Print(string prefix=""){ // 打印
		size_t sz = ChildSize();
		cout<< prefix+"List: size="<<sz<<endl;
		for(size_t i=0; i<sz; i++)
		{
			Child(i)->Print(prefix+PRT);
		}
	};
};

// [] 字典数据结构
struct NodeDict : NodeTree{
	NodeDict(Word &w)
	: NodeTree(NT::Dict, w){}
	inline void Print(string prefix=""){ // 打印
		size_t sz = ChildSize();
		cout<< prefix+"Dict: size="<<sz<<endl;
		for(size_t i=0; i<sz; i++)
		{
			Child(i)->Print(prefix+PRT);
		}
	};
};

// {} 块结构
struct NodeBlock : NodeTree{
	NodeBlock(Word &w)
	: NodeTree(NT::Block, w){}
	inline void Print(string prefix=""){ // 打印
		size_t sz = ChildSize();
		cout<< prefix+"Block: size="<<sz<<endl;
		for(size_t i=0; i<sz; i++)
		{
			Child(i)->Print(prefix+PRT);
		}
	};
};







// : 赋值节点
struct NodeAssign: NodeTwinTree{
	NodeAssign(Word &w)
	: NodeTwinTree(NT::Assign, w){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"Assign(:)"<<endl;
		Left()->Print(prefix+PRT);
		Right()->Print(prefix+PRT);
	};
};

// :: 向上查找赋值节点
struct NodeAssignUp: NodeTwinTree{
	NodeAssignUp(Word &w)
	: NodeTwinTree(NT::AssignUp, w){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"AssignUp(::)"<<endl;
		Left()->Print(prefix+PRT);
		Right()->Print(prefix+PRT);
	};
};


// + 加操作节点
struct NodeAdd: NodeTwinTree{
	NodeAdd(Word &w)
	: NodeTwinTree(NT::Add, w){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"Add(+)"<<endl;
		Left()->Print(prefix+PRT);
		Right()->Print(prefix+PRT);
	};
};


// - 减操作节点
struct NodeSub: NodeTwinTree{
	NodeSub(Word &w)
	: NodeTwinTree(NT::Sub, w){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"Sub(-)"<<endl;
		// 没有左叶则为取负操作
		if(Left()) Left()->Print(prefix+PRT);
		Right()->Print(prefix+PRT);
	};
};

// * 乘操作节点
struct NodeMul: NodeTwinTree{
	NodeMul(Word &w)
	: NodeTwinTree(NT::Mul, w){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"Mul(*)"<<endl;
		Left()->Print(prefix+PRT);
		Right()->Print(prefix+PRT);
	};
};

// / 除操作节点
struct NodeDiv: NodeTwinTree{
	NodeDiv(Word &w)
	: NodeTwinTree(NT::Div, w){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"Div(/)"<<endl;
		Left()->Print(prefix+PRT);
		Right()->Print(prefix+PRT);
	};
};


// print 打印变量至屏幕
struct NodePrint : NodeOneTree{
	NodePrint(Word &w)
	: NodeOneTree(NT::Print, w){}
	inline void Print(string prefix=""){ // 打印
		cout<<prefix+"Print: "<<endl;
		if(child) child->Print(prefix+PRT);
	};
};


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
// --end-- DEF_NODE_H

