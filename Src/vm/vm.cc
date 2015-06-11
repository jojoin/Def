/**
 * Def 虚拟机（解释器）
 */

#include <iostream>
#include <cstdlib>

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

// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);


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
    // cout << words->at(2).value << endl;

    // 语法分析
    Nodezer N(words, ispath ? txt : ""); // 初始化语法分析器
    Node *node = N.BuildAST(); // 解析得到语法树（表达式）

    // cout << node->Child(0)->GetName() << endl;
    // cout << node->Right()->Child(1)->Left()->GetName() << endl;

    // node->Print();
    // cout<<endl;
    // cout << "node->ChildSize() = " << node->ChildSize() << endl;


    // 解释执行分析树
    bool done = ExplainAST(node);
    
    delete words; // 析构words数组
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


//返回对象
inline ObjectNone* Vm::NewObjNone()
{
    return vm_gc->prep_none;
}
inline ObjectBool* Vm::NewObjTrue()
{
    return vm_gc->prep_true;
}
inline ObjectBool* Vm::NewObjFalse()
{
    return vm_gc->prep_false;
}

/**
 * 对语法节点进行求值操作
 */
DefObject* Vm::Evaluat(Node* n)
{

    if(n==NULL) return NULL;

#define T NodeType

    T t = n->type; //当前节点类型

    if(t==T::Assign){ // 赋值
        return Assign(n);

    }else if(t==T::Variable){ // 通过名字取得变量值
        //cout<<"Variable !!!"<<endl;
        return vm_stack->VarGet(n->GetName());

    }else if(t==T::ProcDefine){ // 处理器定义
        //cout<<"ProcDefine !!!"<<endl;
        return DefineProc(n);

    }else if(t==T::List){ // list 数组
        //cout<<"List !!!"<<endl;
        return StructList(n);

    }else if(t==T::Dict){ // dict 字典
        //cout<<"Dict !!!"<<endl;
        return StructDict(n);

    }else if(t==T::Print){ // print 打印
        // cout<<"print!!!"<<endl;
        return Print( n->Child() );

    }else if(t==T::Priority){ // priority 括号优先级
        // cout<<"priority!!!"<<endl;
        return Evaluat( n->Child() ); //递归

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
        
    }else if(t==T::ContainerAccess){ // 容器访问
        // cout<<"ContainerAccess !!!"<<endl;
        return ContainerAccess(n);


    }else{


    }

#undef T   // NodeType

}


#define OT ObjectType
#define NT NodeType



/**
 * 赋值操作
 */
DefObject* Vm::Assign(Node*n)
{
    DefObject *rv = Evaluat(n->Right());   // 等号右值
    Node* nl = n->Left();
    NT nt = nl->type;

    // 普通变量赋值
    if(nt==NT::Variable){
        // cout<<"Assign name="<<name<<endl;
        string name = nl->GetName();     // 名字
        DefObject *exi = vm_stack->VarGet(name);   // 查找变量是否存在
        if(exi){
            // cout<<"vm_gc->Free()"<<endl;
            Free(exi);       // 变量重新赋值则释放之前的变量
        }
        // cout<<"vm_stack->VarPut()"<<name<<endl;
        vm_stack->VarPut(name, rv);   // 变量入栈
        vm_gc->Quote(rv); // 引用计数 +1

    // 容器访问赋值
    }else if(nt==NT::ContainerAccess){

        DefObject *con = Evaluat( nl->Left() ); // 得到容器
        OT ct = con->type; // 容器类型
        Node *idx = nl->Right(); // 索引
        size_t idx_sz = idx ? idx->ChildSize() : 0;

        // 字典
        if(ct==OT::Dict){ 
            ObjectDict *dict = (ObjectDict*)con;
            if(!idx_sz){
                return rv; // do nothing
            }
            string key = Conversion::String( Evaluat( idx->Child(0) ) );
            DefObject *exi = dict->Visit(key);
            if(exi){ //已存在，则解引用
                Free(exi);
            }
            dict->Push(key, rv); // 添加

        // 列表
        }else if(ct==OT::List){
            ObjectList *list = (ObjectList*)con;
            if(!idx_sz){ // 添加到末尾
                list->Push(rv);
            }else{ // 替换制定位置
                DefObject *oi = Evaluat( idx->Child(0) );
                if(oi->type!=OT::Int){
                    return rv; // do nothing
                }
                size_t i = Conversion::Long( oi );
                if(i<0){
                    return rv; // do nothing
                }
                DefObject *exi = list->Visit(i);
                if(exi){ //已存在，则解引用
                    Free(exi);
                }
                list->Push(i, rv); // 添加到指定位置
                
            }



        }

    }

    vm_gc->Quote(rv); // 引用计数 +1
    return rv; // 返回右值
}


/**
 * 算法操作
 * @param opt 算法种类 + - * /
 */
DefObject* Vm::Operate(Node *nl, Node *nr, NT t)
{

    // 取负运算
    if( !nl && t==NT::Sub){
        DefObject *r = Evaluat(nr);
        if(r->type==OT::Int){
            return vm_gc->AllotInt( 0 - ((ObjectInt*)r)->value );
        }else if(r->type==OT::Float){
            return vm_gc->AllotInt( 0.0 - ((ObjectFloat*)r)->value );
        }
        ERR("Err:  only <Int> and <Float> type can get negative value !");
    }

    // 正式运算
    DefObject *l = Evaluat(nl);
    DefObject *r = Evaluat(nr);
    DefObject *result = NULL;

    OT lt = l->type;
    OT rt = r->type;

    // 整数算法
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

    }else if(t==OT::List){ // 列表

        //cout<<"-Print List-"<<endl;
        ObjectList* list = (ObjectList*)obj;
        cout << "(";
        //size_t sz = obj->Size();
        size_t sz = list->Size();
        for(size_t i=0; i<sz; i++){
            if(i) cout<<" ";
            Print( list->Visit(i) );
        }
        cout << ")";

    }else if(t==OT::Dict){ // 字典

        //cout<<"-Print Dict-"<<endl;
        ObjectDict* dict = (ObjectDict*)obj;
        cout << "[";
        map<string, DefObject*>::iterator it = dict->value.begin();
        bool dv = false;
        for(;it!=dict->value.end();++it){
            if(dv) cout<<", "; else dv=true;
            cout<<"'"<<it->first<<"'";
            Print( it->second );   
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
 * If 控制结构
 */
DefObject* Vm::ControlWhile(Node* n)
{
    NodeWhile* p = (NodeWhile*)n;
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
DefObject* Vm::ControlIf(Node* n)
{
    NodeIf* p = (NodeIf*)n;
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



/**
 * list 列表结构建立
 */
DefObject* Vm::StructList(Node* n)
{
    NodeList* p = (NodeList*)n;

    ObjectList* list = vm_gc->AllotList();
    size_t i = 0
         , s = p->ChildSize();
    while( i < s ){
        // 添加数组项目
        list->Push( Evaluat( p->Child(i) ) );
        i++;
    }

    return list;
}


/**
 * dict 数据结构建立
 */
DefObject* Vm::StructDict(Node* n)
{
    NodeDict* p = (NodeDict*)n;

    ObjectDict* dict = vm_gc->AllotDict();
    size_t i = 0
         , s = p->ChildSize();
    while( i < s ){
        // 添加数组项目
        string key = Conversion::String( Evaluat( p->Child(i) ) );
        if(key!=""){
            dict->Push( 
                key, 
                Evaluat( p->Child(i+1) )
            );
        }
        i += 2;
    }

    return dict;
}




/**
 * def 处理器定义
 */
DefObject* Vm::DefineProc(Node* n)
{
    NodeProcDefine* p = (NodeProcDefine*)n;


    return NULL;
}





/**
 * ContainerAccess 容器访问
 */
DefObject* Vm::ContainerAccess(Node* n)
{
    // cout<<"-Vm::ContainerAccess-"<<endl;
    NodeContainerAccess* p = (NodeContainerAccess*)n;

    Node* con = p->Left();
    Node* idx = p->Right();
    size_t idxlen = idx->ChildSize();

    DefObject* result = NULL; //容器访问结果

    // cout<<"idxlen="<<idxlen<<endl;
    if(idxlen){ //索引个数

        DefObject* obj = Evaluat( con );
        OT ot = obj->type;

        if(ot==OT::Dict){ // 字典访问
            string key = Conversion::String( Evaluat( idx->Child(0) ) );
            // cout<<"key = "<<key<<endl;
            result = ((ObjectDict*)obj)->Visit(key); 

        }else if(ot==OT::List){ // 列表访问
            if(idxlen==1){ // 单个获取
                size_t i = Conversion::Long( Evaluat( idx->Child(0) ) );
                // cout<<"idx = "<<i<<endl;
                result = ((ObjectList*)obj)->Visit(i);
            }
        }
    }

    // cout<<"return result="<<(int)result<<endl;
    return result ? result : NewObjNone(); // 无效访问 返回 none
}
















#undef OT   // ObjectType
#undef NT   // NodeType



} // end namespace de
} // end namespace vm
