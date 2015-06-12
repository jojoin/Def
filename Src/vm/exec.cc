/**
 * Def 执行上下文
 */

#include <iostream>
#include <cstdlib>
#include <direct.h>

#include "exec.h"

using namespace std;

using namespace def::parse;
using namespace def::object;


namespace def {
namespace vm {

// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);


#define OT ObjectType
#define NT NodeType



/**
 * 构造函数，采用统一的Gc
 */
Exec::Exec(Node* n, Gc* g, Module* m, ExecType t )
: _node( n )
, _gc( g )
, _mod( m )
, type( t )
{
    _stack = new Stack(); // 新建执行栈
}


/**
 * 拷贝构造
 */
Exec::Exec(Exec&e, ExecType t )
{
	Exec(e._node, e._gc, e._mod, t);
}


// 执行栈变量初始化
void Exec::StackPush(string name, DefObject* obj)
{
	_stack->VarPut(name, obj);
}



// 指定&获取父栈
Stack* Exec::StackParent(Stack*p)
{
	return _stack->parent = p;
}       


/**
 * 执行 Def 调用帧
 * @return 表示解释成功或失败
 */
DefObject* Exec::Run()
{
    if(!_node || !_gc){
    	ERR("Err: _node or _gc is NULL !");
    }

    // 组合表达式 NodeType::Group
    size_t i = 0
         , s = _node->ChildSize();
    while(i<s){
        Evaluat( _node->Child(i) );
        i++;
    }

    return NULL;
}


/**
 * 执行当前栈帧的垃圾回收
 * 通常在一句表达式执行完毕后调用
 */
inline bool Exec::Free(DefObject *obj)
{
    return _gc->Free(obj);
}


//返回对象
inline ObjectNone* Exec::NewObjNone()
{
    return _gc->prep_none;
}
inline ObjectBool* Exec::NewObjTrue()
{
    return _gc->prep_true;
}
inline ObjectBool* Exec::NewObjFalse()
{
    return _gc->prep_false;
}

/**
 * 对语法节点进行求值操作
 */
DefObject* Exec::Evaluat(Node* n)
{

    if(n==NULL) return NULL;

#define T NodeType

    T t = n->type; //当前节点类型

    if(t==T::Group){ // 语句组（if或while的body）
    	DefObject* last = NULL;
	    size_t i = 0
	         , s = _node->ChildSize();
	    while(i<s){
	        last = Evaluat( _node->Child(i) );
	        i++;
	    }
	    return last; //返回最后一条语句的值

    }else if(t==T::Variable){ // 通过名字取得变量值
        //cout<<"Variable !!!"<<endl;
        string name = n->GetName();
        DefObject* val = _stack->VarGet( name );
        if(!val){ // 变量不存在
        	ERR("Can't find variable : "+name+" !");
        }
        return val;

    }else if(t==T::Priority){ // 优先级
        // cout<<"priority!!!"<<endl;
        return Evaluat( n->Child() ); //递归求值

#define ELSE_IF(kind)  }else if(t==T::kind){ return kind(n);
        ELSE_IF(Assign)
        ELSE_IF(AssignUp)
        ELSE_IF(ProcDefine)
        ELSE_IF(List)
        ELSE_IF(Dict)
        ELSE_IF(Print)
        ELSE_IF(If)
        ELSE_IF(While)
        ELSE_IF(ContainerAccess)
        ELSE_IF(Import)
#undef ELSE_IF

    }else if(t==T::Add||t==T::Sub||t==T::Mul||t==T::Div){ // + - * / 算法操作
        //cout<<"add sub mul div !!!"<<endl;
        return Operate( n->Left(), n->Right(), t);

    }else if(t==T::None||t==T::Bool||t==T::Int||t==T::Float||t==T::String){ // none bool int 字面量求值
        // cout<<"Allot !!!"<<endl;
        return _gc->Allot(n);
        
    }else{

    	// 未定义的操作
    	ERR("No Evaluat Match !");

    }

#undef T   // NodeType

}



/**
 * 赋值操作
 */
DefObject* Exec::Assign(Node*n)
{
    DefObject *rv = Evaluat(n->Right());   // 等号右值
    Node* nl = n->Left();
    NT nt = nl->type;

    // 普通变量赋值
    if(nt==NT::Variable){
        // cout<<"Assign name="<<name<<endl;
        string name = nl->GetName();     // 名字
        DefObject *exi = _stack->VarGet(name);   // 查找变量是否存在
        if(exi){
            // cout<<"_gc->Free()"<<endl;
            Free(exi);       // 变量重新赋值则释放之前的变量
        }
        // cout<<"_stack->VarPut()"<<name<<endl;
        _stack->VarPut(name, rv);   // 变量入栈
        _gc->Quote(rv); // 引用计数 +1

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
                ERR("Dict Need <string> type key on Assign !");
                return rv; // do nothing
            }
            DefObject *ik = Evaluat( idx->Child(0) );
            if(ik->type!=OT::String){ // 验证 key 类型
                ERR("Dict key only <string> type on Assign !");
            }
            string key = Conversion::String( ik );
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
                    ERR("List index only <int> type on Assign !");
                    return rv; // do nothing
                }
                size_t i = Conversion::Long( oi );
                if(!i){
                    ERR("List index begin from <int> 1 !");
                    return rv; // do nothing
                }
                DefObject *exi = list->Visit(i-1); //索引从1开始
                if(exi){ //已存在，则解引用
                    Free(exi);
                }
                list->Push(i-1, rv); // 添加到指定位置

            }



        }

    }

    _gc->Quote(rv); // 引用计数 +1
    return rv; // 返回右值
}



/**
 * 赋值操作
 */
DefObject* Exec::AssignUp(Node*n)
{

	return NULL;
}




/**
 * 算法操作
 * @param opt 算法种类 + - * /
 */
DefObject* Exec::Operate(Node *nl, Node *nr, NT t)
{

    // 取负运算
    if( !nl && t==NT::Sub){
        DefObject *r = Evaluat(nr);
        if(r->type==OT::Int){
            return _gc->AllotInt( 0 - ((ObjectInt*)r)->value );
        }else if(r->type==OT::Float){
            return _gc->AllotInt( 0.0 - ((ObjectFloat*)r)->value );
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
        result = _gc->AllotInt(res);

    }else if( lt==OT::String && lt==OT::String ){
        string str = ((ObjectString*)l)->value 
                   + ((ObjectString*)r)->value;
        if(t==NT::Add){ //字符串 +
            result = _gc->AllotString(str);
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
DefObject* Exec::Print(Node *n)
{
    DefObject* obj = Evaluat( n->Child() );
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
DefObject* Exec::Print(DefObject *obj)
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
DefObject* Exec::While(Node* n)
{
    NodeWhile* p = (NodeWhile*)n;
    while(1){
        if(Conversion::Bool( Evaluat( p->Left() ) )){
            Evaluat( p->Right() ); //执行 while 块
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
DefObject* Exec::If(Node* n)
{
    NodeIf* p = (NodeIf*)n;
    size_t i = 0
         , s = p->ChildSize();
    while(1){
        if(i+1==s){  //执行 else 块
            Evaluat( p->Child(i) );
            break;
        }
        if(i>=s){
            break; // 结束
        }
        if(Conversion::Bool( Evaluat( p->Child(i) ) )){
            Evaluat( p->Child(i+1) ); //执行 if 块
            break;
        }
        i += 2;
    }
    return NULL;
}



/**
 * list 列表结构建立
 */
DefObject* Exec::List(Node* n)
{
    NodeList* p = (NodeList*)n;

    ObjectList* list = _gc->AllotList();
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
DefObject* Exec::Dict(Node* n)
{
    NodeDict* p = (NodeDict*)n;

    ObjectDict* dict = _gc->AllotDict();
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
DefObject* Exec::ProcDefine(Node* n)
{
    NodeProcDefine* p = (NodeProcDefine*)n;


    return NULL;
}



/**
 * ContainerAccess 容器访问
 */
DefObject* Exec::ContainerAccess(Node* n)
{
    // cout<<"-Exec::ContainerAccess-"<<endl;
    NodeContainerAccess* p = (NodeContainerAccess*)n;

    Node* con = p->Left();
    Node* idx = p->Right();
    size_t idxlen = idx->ChildSize();

    DefObject* result = NULL; //容器访问结果

    // cout<<"idxlen="<<idxlen<<endl;
    if(idxlen){ //索引个数

        DefObject* obj = Evaluat( con );
        OT ot = obj->type;
        DefObject* i1 = Evaluat( idx->Child(0) );
        OT i1t = i1->type;

        if(ot==OT::Dict){ // 字典访问
            if(i1t!=OT::String){
                ERR("Dict key only <string> type !");
            }
            string key = Conversion::String( i1 );
            // cout<<"key = "<<key<<endl;
            result = ((ObjectDict*)obj)->Visit(key); 

        }else if(ot==OT::List){ // 列表访问
            if(i1t!=OT::Int){
                ERR("List index only <int> type !");
            }
            if(idxlen==1){ // 单个获取
                size_t i = Conversion::Long( i1 );
                if(!i){
                    ERR("List index begin from <int> 1 !");
                }
                result = ((ObjectList*)obj)->Visit(i-1);
            }
        }
    }

    // cout<<"return result="<<(int)result<<endl;
    return result ? result : NewObjNone(); // 无效访问 返回 none
}




/**
 * Import 模块加载
 */
DefObject* Exec::Import(Node* n)
{
    // cout<<"-Exec::Import-"<<endl;
	DefObject* name = Evaluat( n->Child() );

	if(name->type!=OT::String){
		ERR("Module import only <string> name or file path !");
	}

	// 加载模块
	string mdname = Conversion::String( name );
	ObjectModule* md = _mod->Load(
		mdname, ""
	);

	if(!md){
		// ERR("Can't find the module : "+mdname);
	}


	return md;
}










#undef OT   // ObjectType
#undef NT   // NodeType



} // end namespace vm
} // end namespace def



