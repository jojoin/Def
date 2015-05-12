/**
 * Def 虚拟机（解释器）
 */

#include <iostream>

#include "vm.h"

using namespace std;

using namespace def::error;
using namespace def::token;
using namespace def::node;
using namespace def::object;
using namespace def::operat;
using namespace def::stack;
using namespace def::gc;

namespace def {
namespace vm {


Vm::Vm(){
    vm_stack = new Stack(); // 新建执行栈
    vm_gc = new Gc(); // 新建对象分配
}


/**
 * 运行时错误
 */
bool Vm::Error(int code){

    // string msg = filepath + " ("
    //     +Str::l2s(line) + ","
    //     +Str::l2s(word_pos)
    //     +") : " + tok;
    // //tok;
    // return Error::Throw(ErrorType::Run, code, msg);
    return true;
}



/**
 * 运行 Def 语言脚本
 * @return 表示运行成功或失败
 */
bool Vm::Eval(string txt, bool ispath=false)
{
    // 词法分析结果
    vector<Word>* words = new vector<Word>(1024);
    Tokenizer T(ispath, txt, words); // 初始化词法分析器
    T.Scan(); // 执行词法分析

    // cout << words->at(0).value << endl;
    // cout << words->at(1).value << endl;

    // 语法分析
    Nodezer N(words, ispath ? txt : ""); // 初始化语法分析器
    Node *node = N.BuildAST(); // 解析得到语法树（表达式）

    // cout << node->Child(0)->GetName() << endl;
    // cout << node->Right()->Child(1)->Left()->GetName() << endl;

    node->Print();
    // cout << "node->ChildSize() = " << node->ChildSize() << endl;


    // 解释执行分析树
    // bool done = ExplainAST(node);
    
    delete words; // 析构tok数组
    delete node; // 析构语法树
    
	return true;
}


/**
 * 解释执行 Def 语法树
 * @return 表示解释成功或失败
 */
bool Vm::ExplainAST(Node *p)
{
    if(p==NULL) return false;

    // 组合表达式 NodeType::Group
    size_t i = 0
         , s = p->ChildSize();
    while(i<s){
        Evaluat( p->Child(i) );
        i++;
    }

    return true;
}


/**
 * 执行当前栈帧的垃圾回收
 * 通常在一句表达式执行完毕后调用
 */
inline bool Vm::Free(DefObject *obj)
{
    return vm_gc->Free(obj);
}



/**
 * 登记新创建的变量，用于集中垃圾回收
 *
bool Vm::Regist(DefObject *obj)
{
    return true;
    // 登记
    size_t size = vm_stack->Regist(obj);
    //cout<<"Vm::Regist() "<<size<<endl;
    // 垃圾回收触发条件
    if(size > 10){
        Clean(); // 回收
    }
    return true;
}
*/


/**
 * 对语法节点进行求值操作
 */
DefObject* Vm::Evaluat(Node* n)
{

    if(n==NULL) return NULL;

#define T NodeType

    T t = n->type; //当前节点类型

    if(t==T::Assign){ // 赋值

        DefObject *rv = Evaluat(n->Right());   // 等号右值
        string name = n->Left()->GetName();     // 名字
        // cout<<"Assign name="<<name<<endl;
        DefObject *exi = vm_stack->VarGet(name);   // 查找变量是否存在
        if(exi!=NULL){
            // cout<<"vm_gc->Free()"<<endl;
            Free(exi);       // 变量重新赋值则释放之前的变量
        }
        vm_gc->Quote(rv);          // 引用计数 +1
        // cout<<"vm_stack->VarPut()"<<name<<endl;
        vm_stack->VarPut(name, rv);   // 变量入栈
        return rv;

    }else if(t==T::Variable){ // 通过名字取得变量值
        return vm_stack->VarGet(n->GetName());

    }else if(t==T::List){ // list 数组

        //cout<<"List !!!"<<endl;
        return StructList(n);

    }else if(t==T::Dict){ // dict 字典


    }else if(t==T::Print){ // print 打印

        // cout<<"print!!!"<<endl;
        return Print( n->Right() ); // 打印

    }else if(t==T::Add||t==T::Sub||t==T::Mul||t==T::Div){ // + - * / 算法操作

        //cout<<"add sub mul div !!!"<<endl;
        return Operate( n->Left(), n->Right(), t);

    }else if(t==T::If){ // if 条件结构

        //cout<<"if !!!"<<endl;
        return ControlIf(n);

    }else if(t==T::While){ // while 循环结构

        //cout<<"While !!!"<<endl;
        return ControlWhile(n);

    }else if(t==T::None||t==T::Bool||t==T::Int||t==T::Float||t==T::String){ // none bool int 字面量求值

        // cout<<"Allot !!!"<<endl;
        return vm_gc->Allot(n);
        //DefObject *crt = vm_gc->Allot(n);   // 分配新变量
        //Regist(crt);    // 登记新变量
        //return crt;

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

    DefObject *l = Evaluat(nl);
    DefObject *r = Evaluat(nr);
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

    }else if( lt==OT::String && lt==OT::String ){
        string str = ((ObjectString*)l)->value 
                   + ((ObjectString*)r)->value;
        if(t==NT::Add){ //字符串 +
            result = vm_gc->AllotString(str);
        }

    }else{



    }

    // 参与计算的临时变量的释放
    if( nl->IsValue() || nl->IsOperate() ){
        //cout<<"nl->type==NT::Int"<<endl;
        Free(l);
    }
    if( nr->IsValue() || nr->IsOperate() ){
        //cout<<"nr->type==NT::Int"<<endl;
        Free(r);
    }


    return result;
}


/**
 * 打印对象
 */
DefObject* Vm::Print(Node *n)
{
    DefObject* obj = Evaluat(n);
    Print( obj ); // 求值并打印
    cout << endl;

    // 临时变量释放
    if( n->IsValue() || n->IsOperate() ){
        //cout<<"Free Literals or Algorithm Value"<<endl;
        Free(obj);
    }

    return obj;
}



/**
 * 打印对象
 */
DefObject* Vm::Print(DefObject *obj)
{
    OT t = obj->type; // 获取类型

    if( t==OT::Int ){

        //cout<<"-Print Int-"<<endl;
        cout << ((ObjectInt*)obj)->value;

    }else if(t==OT::String){

        cout << "\"";
        cout << ((ObjectString*)obj)->value;
        cout << "\"";

    }else if(t==OT::List){

        //cout<<"-Print List-"<<endl;
        ObjectList* list = (ObjectList*)obj;
        cout << "[";
        //size_t sz = obj->Size();
        size_t sz = list->Size();
        for(size_t i=0; i<sz; i++){
            if(i>0){
                cout<<" ";
            }
            Print( list->Visit(i) );
        }
        cout << "]";

    }else if(t==OT::None){
        cout << "none";
    }else if(t==OT::Bool){
        if( Conversion::Bool(obj) ){
            cout << "true";
        }else{
            cout << "false";
        }
    }else{

    }

    return obj;
}





/**
 * list 数据结构建立
 */
DefObject* Vm::StructList(Node* p)
{
    p = (NodeList*)p;

    ObjectList* list = vm_gc->AllotList();
    size_t i = 0
         , s = p->ChildSize();
    while( i < s ){
        // 添加数组项目
        list->Push( vm_gc->Allot( p->Child(i) ) );
        i++;
    }

    return list;
}



/**
 * If 控制结构
 */
DefObject* Vm::ControlWhile(Node* p)
{
    p = (NodeWhile*)p;
    while(1){
        if(Conversion::Bool( Evaluat( p->Left() ) )){
            ExplainAST( p->Right() ); //执行 while 块
            // cout<<"\n"<<endl;
        }else{
            break; 
        }
    }
    return NULL; 
}

/**
 * If 控制结构
 */
DefObject* Vm::ControlIf(Node* p)
{
    p = (NodeIf*)p;
    size_t i = 0
         , s = p->ChildSize();
    while(1){
        if(i+1==s){  //执行 else 块
            ExplainAST( p->Child(i) );
            break;
        }
        if(i>=s){
            break; // 结束
        }
        if(Conversion::Bool( Evaluat( p->Child(i) ) )){
            ExplainAST( p->Child(i+1) ); //执行 if 块
            break;
        }
        i += 2;
    }
    return NULL;
}






#undef OT   // ObjectType
#undef NT   // NodeType



} // end namespace de
} // end namespace vm
