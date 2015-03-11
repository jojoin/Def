/**
 * Def 虚拟机（解释器）
 */

#include <iostream>

#include "vm.h"
#include "../object/operat.h"

using namespace std;

using namespace def::token;
using namespace def::node;
using namespace def::object;
using namespace def::operat;
using namespace def::stack;
using namespace def::gc;
using namespace def::vm;



Vm::Vm(){
    vm_stack = new Stack(); // 新建执行栈
    vm_gc = new Gc(); // 新建对象分配
}

/**
 * 运行 Def 语言脚本
 * @return 表示运行成功或失败
 */
bool Vm::Eval(string txt, bool ispath=false)
{
    // 词法分析
    vector<Word> words; // 词法分析结果
    Tokenizer T(ispath, txt, words); // 初始化词法分析器
    T.Scan(); // 执行词法分析

    // 语法分析
    Nodezer N(words); // 初始化语法分析器
    Node *node = N.BuildAST(); // 解析得到语法树（表达式）

    // 解释执行分析树
    bool done = Execute(node);

    /*
    cout <<
    node->Child(2)->Right()->GetName()
    << endl;
    */
    
    

    delete node; // 析构语法树
    
	return true;
}


/**
 * 解释执行 Def 语法树
 * @return 表示解释成功或失败
 */
bool Vm::Execute(Node *p)
{
    if(p==NULL) return false;

    // 组合表达式 NodeType::Group
    size_t i = 0
         , s = p->ChildSize();
    while(i<s){
        GetValue( p->Child(i) );
        i++;
    }

    return true;
}


/**
 * 对语法节点进行求值操作
 */
DefObject* Vm::GetValue(Node* n)
{

    if(n==NULL) return NULL;

#define T NodeType

    T t = n->type; //当前节点类型

    if(t==T::Assign){ // 赋值

        DefObject *rv = GetValue(n->Right());   // 等号右值
        string name = n->Left()->GetName();     // 名字
        //cout<<"Assign name="<<name<<endl;
        DefObject *exi = vm_stack->Get(name);   // 查找变量是否存在
        if(exi!=NULL){
            //cout<<"vm_gc->Free()"<<endl;
            vm_gc->Free(exi);       // 变量重新赋值则释放之前的变量
        }
        vm_gc->Quote(rv);          // 引用计数 +1
        vm_stack->Put(name, rv);   // 变量入栈
        return rv;

    }else if(t==T::Variable){ // 通过名字取得变量值

        return vm_stack->Get(n->GetName());

    }else if(t==T::Print){ // print 打印

        //cout<<"print!!!"<<endl;
        return Print( n->Right() ); // 打印

    }else if(t==T::Add||t==T::Sub||t==T::Mul||t==T::Div){ // + - * / 算法操作

        //cout<<"add sub mul div !!!"<<endl;
        return Operate( n->Left(), n->Right(), t);

    }else if(t==T::If){ // if 条件结构

        //cout<<"if !!!"<<endl;
        return ControlIf((NodeIf*)n);

    }else if(t==T::While){ // while 循环结构

        //cout<<"While !!!"<<endl;
        return ControlWhile((NodeWhile*)n);

    }else if(t==T::None||t==T::Bool||t==T::Int){ // none bool int 字面量求值

        DefObject *crt = vm_gc->Allot(n);   // 分配新变量
        vm_stack->Regist(crt);    // 登记新变量
        return crt;

    }else{

    }

#undef T   // NodeType

}


#define OT ObjectType
#define NT NodeType


/**
 * 算法操作
 * @param opt 算法种类 + - * /
 */
DefObject* Vm::Operate(Node *nl, Node *nr, NT t)
{

    DefObject *l = GetValue(nl);
    DefObject *r = GetValue(nr);
    DefObject *result = NULL;

    OT lt = l->type;
    OT rt = r->type;

    // 整数相加
    if( lt==OT::Int && lt==OT::Int ){

        long vl = ((ObjectInt*)l)->value
           , vr = ((ObjectInt*)r)->value
           , res= 0;
        switch(t){
        case NT::Add: res = vl + vr; break;
        case NT::Sub: res = vl - vr; break;
        case NT::Mul: res = vl * vr; break;
        case NT::Div: res = vl / vr; break;
        }
        result = vm_gc->AllotInt(res);

    }else{

    }

    vm_stack->Regist(result);    // 登记新变量
    return result;
}



/**
 * 打印对象
 */
DefObject* Vm::Print(Node *n)
{
    DefObject* obj = GetValue(n); // 求值

    OT t = obj->type; // 获取类型

    if( t==OT::Int ){ // 整数

        cout << ((ObjectInt*)obj)->value << endl;

    }else{

    }

    return obj;
}


/**
 * If 控制结构
 */
DefObject* Vm::ControlWhile(NodeWhile *p)
{
    while(1){
        if(Conversion::Bool( GetValue( p->Left() ) )){
            Execute( p->Right() ); //执行 while 块
        }else{
            break;
        }
    }
    return NULL;
}

/**
 * If 控制结构
 */
DefObject* Vm::ControlIf(NodeIf *p)
{
    size_t i = 0
         , s = p->ChildSize();
    while(1){
        if(i+1==s){  //执行 else 块
            Execute( p->Child(i) );
            break;
        }
        if(i>=s){
            break; // 结束
        }
        if(Conversion::Bool( GetValue( p->Child(i) ) )){
            Execute( p->Child(i+1) ); //执行 if 块
            break;
        }
        i += 2;
    }
    return NULL;
}


#undef OT   // ObjectType
#undef NT   // NodeType


/****** 脚本解释器测试 ******/


int main()
{
    //cout << "\n";

    Vm v = Vm(); // 初始化引擎
    v.Eval("./test.d", true);

    //cout << "\n\n";
}


/****** 测试结束 ******/