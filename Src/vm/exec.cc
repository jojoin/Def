/**
 * Def 执行上下文
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <map>

#include "../global.h"

#include "exec.h"
#include "../util/str.h"
#include "../util/fs.h"
#include "../util/path.h"

using namespace std;

using namespace def::parse;
using namespace def::object;
using namespace def::util;


namespace def {
namespace vm {

// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);


#define DO DefObject
#define OT ObjectType
#define NT NodeType

// 将环境变量初始化
#define LOCALIZE_module    Module *_module = _envir._module;
#define LOCALIZE_gc        Gc     *_gc     = _envir._gc;
#define LOCALIZE_stack     Stack  *_stack  = _envir._stack;
#define LOCALIZE_node      Node   *_node   = _envir._node;


/**
 * 完全构造
 */
Exec::Exec(void)
{
    _envir = Envir(); // 新建执行环境
}

/**
 * 拷贝构造执行环境
 */
Exec::Exec(Envir e){
	_envir = Envir(e);
}


// 执行栈变量初始化
void Exec::StackPush(string name, DO* obj)
{
	_envir._stack->VarPut(name, obj);
}



// 指定&获取父栈
Stack* Exec::StackParent(Stack*p)
{
	return _envir._stack->parent = p;
}       




/**
 * 从入口文件开始执行
 */
bool Exec::Main(string fl)
{
	string file = Path::join(Path::cwd(), fl);

	if(!Fs::Exist(file)){
		ERR("File \""+file+"\" is not find !");
	}

	string text = Fs::ReadFile(file);
	// cout<<file<<endl;
	_envir.SetFile(file); // 入口文件

	Node *nd = Parse(text, file); // 解析语法
	// nd->Print();
    // return false; 
	_envir.Set(nd);       // 设置环境

    // 解释执行
    Run();

    // 清除环境数据
    _envir.Clear();

    return true;

}


/**
 * 解析得到抽象语法树
 */
Node* Exec::Parse(string &text, string file)
{

    // 词法分析结果
    Tokenizer T(text); // 初始化词法分析器
    T.SetFile(file);
    vector<Word>* words = T.Scan(); // 执行词法分析

    // cout << words->at(0).value << endl;
    // cout << words->at(1).value << endl;
    // cout << words->at(2).value << endl;

    // 语法分析
    Nodezer N(words); // 初始化语法分析器
    N.SetFile(file);
    Node* node = N.BuildAST(); // 解析得到语法树（表达式）

    // cout << node->Child(0)->GetName() << endl;
    // cout << node->Right()->Child(1)->Left()->GetName() << endl;

    // node->Print();
    // cout<<endl;
    // cout << "node->ChildSize() = " << node->ChildSize() << endl;

    words->clear();
    delete words; // 析构words数组

    return node; // 返回语法树
}



/**
 * 执行代码
 */
DO* Exec::Eval(string code)
{
    code += "      ";
    string file = "file.d";
    // 词法+语法分析
    Node* nd = Parse(code, file);
    // nd->Print();
    // return NULL;
    // 执行语句
    _envir.SetFile(file); // 入口文件
    _envir.Set(nd);       // 设置环境
    // 解释执行
    // Run();
    NodeGroup *gr = (NodeGroup*)nd;
    DO* res = Evaluat( gr->Child(0) );
    // 清除环境数据
    // _envir.Clear();

    // 返回
    return res;
}


/**
 * 执行 Def 调用帧
 * @return 调用的返回对象
 */
DO* Exec::Run()
{
    LOCALIZE_node;
    LOCALIZE_gc;

    if(!_node){
    	return NULL;
    }

    DO *ret = NULL;

    // 组合表达式 NodeType::Group
    size_t i = 0
         , s = _node->ChildSize();
    while(i<s){
        ret = Evaluat( _node->Child(i) );
        i++;
    }

    _gc->Quote(ret); //返回值加引用
    return ret;
}





/**
 * 执行当前栈帧的垃圾回收
 * 通常在一句表达式执行完毕后调用
 */
inline void Exec::VarPut(string name, DO*obj)
{
    LOCALIZE_gc;
    LOCALIZE_stack;

    // 入栈
    DO *exi = _stack->VarPut(name, obj); // 查找变量是否存在
    if(exi){
        // cout<<"_gc->Free()"<<endl;
        _gc->Free(exi);       // 解引用
    }
    // cout<<"_gc->Quote()"<<name<<endl;
    _gc->Quote(obj); // 加引用
}


//返回对象
inline ObjectNone* Exec::ObjNone()
{
    return _envir._gc->prep_none;
}
inline ObjectBool* Exec::ObjTrue()
{
    return _envir._gc->prep_true;
}
inline ObjectBool* Exec::ObjFalse()
{
    return _envir._gc->prep_false;
}

/**
 * 对语法节点进行求值操作
 */
DO* Exec::Evaluat(Node* n)
{
	LOCALIZE_module;
	LOCALIZE_gc;
	LOCALIZE_stack;


    if(n==NULL) return NULL;

#define T NodeType

    T t = n->type; //当前节点类型

    if(t==T::Group)
    { // 语句组（if或while的body）
        NodeGroup *gr = (NodeGroup*)n;
    	DO* last = NULL;
	    size_t i = 0
	         , s = gr->ChildSize();
	    while(i<s){
	        last = Evaluat( gr->Child(i) );
	        i++;
	    }
	    return last; //返回最后一条语句的值

    }
    else if(t==T::Variable)
    {
        //cout<<"Variable !!!"<<endl;
        string name = n->GetName();
        DO* val = Variable(name);
        if(!val){ // 变量不存在
        	ERR("Can't find variable : "+name+" !");
        }
        return val;

    }
    else if(t==T::Priority)
    { // 优先级
        // cout<<"priority!!!"<<endl;
        return Evaluat( n->Child() ); //递归求值

#define IF(kind)  }else if(t==T::kind){ return kind(n);
        IF(Assign)
        IF(AssignUp)
        IF(ProcDefine)
        IF(FuncDefine)
        IF(ProcCall)
        IF(FuncCall)
        IF(Return)
        IF(List)
        IF(Dict)
        IF(Block)
        IF(Print)
        IF(If)
        IF(While)
        IF(ContainerAccess)
        IF(MemberAccess)
        IF(Import)
#undef IF

    }else if(t==T::Add||t==T::Sub||t==T::Mul||t==T::Div){ // + - * / 算法操作
        //cout<<"operate !!!"<<endl;
        return Operate( n->Left(), n->Right(), t);

    }else if(t==T::Equal||t==T::More||t==T::Less||t==T::MoreEqual||t==T::LessEqual||t==T::NotEqual){ // 比较操作
        //cout<<"compare !!!"<<endl;
        return Compare( n->Left(), n->Right(), t);

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
 * 取变量值
 */
DO* Exec::Variable(string name)
{
    // 通过名字取得变量值（支持沿作用域向上查找）
    //cout<<"Variable !!!"<<endl;
    DO* val = _envir._stack->VarGetUp( name );
    return val; // 变量不存在返回 NULL
}




/**
 * 赋值操作
 */
DO* Exec::Assign(Node*n)
{

    Node* nl = n->Left();
    DO* rv = Evaluat( n->Right() );   // 等号右值

    return Assign(nl, rv);
}
// 赋值
DO* Exec::Assign(Node*nl, DO*rv)
{
    LOCALIZE_gc;
    LOCALIZE_stack;

    NT nt = nl->type;
    // 普通变量赋值
    if(nt==NT::Variable){
        // cout<<"Assign name="<<name<<endl;
        string name = nl->GetName();     // 名字
        DO *exi = _stack->VarGet(name);   // 查找变量是否存在
        if(exi){
            // cout<<"_gc->Free()"<<endl;
            _gc->Free(exi);       // 解引用
        }
        // cout<<"_stack->VarPut()"<<name<<endl;
        _stack->VarPut(name, rv);   // 入栈

    // 成员访问赋值
    }else if(nt==NT::MemberAccess){
        // cout<<"MemberAccess Assign name="<<nl->Right()->GetName()<<endl;
        ObjectModule *mod = (ObjectModule*)Evaluat( nl->Left() );
        string member = nl->Right()->GetName();
        DO *exi = mod->Visit(member);
        if(exi){ //已存在，解引用
            _gc->Free(exi);
            mod->Set(member, rv);
        }else{
            mod->Insert(member, rv); // 设置成员
        }

    // 容器访问赋值
    }else if(nt==NT::ContainerAccess){
        // cout<<"ContainerAccess Assign name="<<nl->Right()->GetName()<<endl;
        DO *con = Evaluat( nl->Left() ); // 得到容器
        OT ct = con->type; // 容器类型
        Node *idx = nl->Right(); // 索引
        size_t idx_sz = idx ? idx->ChildSize() : 0;
        // cout<<"idx_sz = "<<idx_sz<<endl;
        // 字典
        if(ct==OT::Dict){
            // cout<<"ct==OT::Dict"<<endl;
            ObjectDict *dict = (ObjectDict*)con;
            if(!idx_sz){
                ERR("Dict Need <string> type key on Assign !");
            }
            // cout<<"*ik = Evaluat("<<endl;
            DO *ik = Evaluat( idx->Child(0) );
            if(ik->type!=OT::String){ // 验证 key 类型
                ERR("Dict key only <string> type on Assign !");
            }
            // cout<<"Conversion::String("<<endl;
            string key = Conversion::String( ik );
            DO *exi = dict->Visit(key);
            // cout<<"exi = "<<exi<<endl;
            if(exi){ //已存在，解引用
                _gc->Free(exi);
                dict->Set(key, rv);
            }else{
                dict->Insert(key, rv); // 设置成员
            }
            // cout<<"dict Assign"<<endl;

        // 列表
        }else if(ct==OT::List){
            ObjectList *list = (ObjectList*)con;
            if(!idx_sz){ // 添加到末尾
                list->Push(rv);
            }else{ // 替换制定位置
                DO *oi = Evaluat( idx->Child(0) );
                if(oi->type!=OT::Int){
                    ERR("List index only <int> type on Assign !");
                    return rv; // do nothing
                }
                size_t i = Conversion::Long( oi );
                if(!i){
                    ERR("List index begin from <int> 1 !");
                    return rv; // do nothing
                }
                DO *exi = list->Visit(i-1); //索引从1开始
                if(exi){ //已存在，解引用
                    _gc->Free(exi);
                }
                list->Push(i-1, rv); // 添加到指定位置

            }
        }
    }

    _gc->Quote(rv); // 加引用
    return rv; // 返回右值

}

/**
 * 赋值操作
 */
DO* Exec::AssignUp(Node*n)
{
    // cout<<"AssignUp "<<endl;
    LOCALIZE_gc;
    LOCALIZE_stack;

    DO *rv = Evaluat( n->Right() );   // 等号右值
    Node* nl = n->Left();
    NT nt = nl->type;

    // 普通变量赋值
    if(nt!=NT::Variable){
        ERR("AssignUp only to <Variable> !")
    }

    string name = nl->GetName();     // 名字

    // false 向上查找 忽略当前栈帧 
    DO* old = _stack->VarPutUp(name, rv, false);
    if(!old){
        ERR("AssignUp can't find variable : \""+name+"\" !")
    }

    // 释放旧变量
    _gc->Free(old);

	return rv;
}




/**
 * 算法操作
 * @param t 算法种类 + - * /
 */
DO* Exec::Operate(Node *nl, Node *nr, NT t)
{
	LOCALIZE_gc;
	LOCALIZE_stack;

    // 取负运算
    if( !nl && t==NT::Sub){
        DO *r = Evaluat(nr);
        if(r->type==OT::Int){
            return _gc->AllotInt( 0 - ((ObjectInt*)r)->value );
        }else if(r->type==OT::Float){
            return _gc->AllotInt( 0.0 - ((ObjectFloat*)r)->value );
        }
        ERR("Err:  only <Int> and <Float> type can get negative value !");
    }

    // 正式运算
    DO *l = Evaluat(nl);
    DO *r = Evaluat(nr);
    DO *result = NULL;

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
        _gc->Free(l);
    }
    if( nr->IsValue() || nr->IsOperate() ){
        //cout<<"nr->type==NT::Int"<<endl;
        _gc->Free(r);
    }


    return result;
}


/**
 * 算法操作
 * @param t 比较种类 = > < >= <= ~= ~
 */
DO* Exec::Compare(Node *nl, Node *nr, NT t)
{
    // 正式运算
    DO *l = Evaluat(nl),
              *r = Evaluat(nr);

    OT lt = l->type,
       rt = r->type;

    DO *_t = ObjTrue(),
              *_f = ObjFalse();

// 数值比较
#define COMPARE(T1,O1,T2,O2)                             \
    if( lt==OT::T1 && rt==OT::T2 ){                      \
    O1 vl = ((Object##T1*)l)->value;                     \
    O2 vr = ((Object##T2*)r)->value;                     \
    switch(t){                                           \
    case NT::Equal:     return vl==vr?_t:_f;             \
    case NT::More:      return vl> vr?_t:_f;             \
    case NT::Less:      return vl< vr?_t:_f;             \
    case NT::MoreEqual: return vl>=vr?_t:_f;             \
    case NT::LessEqual: return vl<=vr?_t:_f;             \
    case NT::NotEqual:  return vl!=vr?_t:_f;             \
    }}

    COMPARE(Int,long,Int,long)
    COMPARE(Int,long,Float,double)
    COMPARE(Float,double,Int,long)
    COMPARE(Float,double,Float,double)

#undef COMPARE

    // 字符串等于、不等于比较
    if( lt==OT::String && rt==OT::String )
    {
        string vl = ((ObjectString*)l)->value;
        string vr = ((ObjectString*)r)->value;
        bool ret = false;
        if(t==NT::Equal) return vl==vr?_t:_f;
        if(t==NT::NotEqual) return vl!=vr?_t:_f;
    }

    // 没有匹配的比较算法
    ERR("Can't compare !");
}




/**
 * 打印对象
 */
DO* Exec::Print(Node *n)
{
    LOCALIZE_gc;

    DO* obj = Evaluat( n->Child() );
    DO::Print( obj ); // 求值并打印
    cout << endl;

    // 临时变量释放
    if( n->IsValue() || n->IsOperate() ){
        //cout<<"Free Literals or Algorithm Value"<<endl;
        _gc->Free(obj);
    }

    return obj;
}


/**
 * If 控制结构
 */
DO* Exec::While(Node* n)
{
    NodeWhile* p = (NodeWhile*)n;
    DO* res = ObjNone(); // 结果
    size_t len = p->ChildSize();
    if(!len){
        return res;
    }
    Node* cond = p->Child(0);
    while(1){
        if(!Conversion::Bool( Evaluat( cond ) )){
            break; // 条件假，跳出循环
        }
        for(int i=1; i<len; i++){
            res = Evaluat( p->Child(i) ); //执行 while 块
        }
    }
    return res; 
}

/**
 * If 控制结构
 */
DO* Exec::If(Node* n)
{
    NodeIf* p = (NodeIf*)n;
    DO* ret = ObjNone();
    size_t i = 0
         , s = p->ChildSize();
    while(i<s){
        NodeGroup *li = (NodeGroup*)p->Child(i);
        size_t n = li->ChildSize();
        if(n){
            Node *cnd = li->Child(0); //条件（值NULL则为else块）
            if(!cnd || Conversion::Bool( Evaluat( cnd ) )){
                for(int j=1; j<n; j++)
                {
                    ret = Evaluat( li->Child(j) ); //执行 if 或 else 块
                }
                break; // if 完成
            }
        }
        i++;
    }
    return ret;
}



/**
 * list 列表结构建立
 */
DO* Exec::List(Node* n)
{
	LOCALIZE_gc

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
DO* Exec::Dict(Node* n)
{
	LOCALIZE_gc

    NodeDict* p = (NodeDict*)n;

    ObjectDict* dict = _gc->AllotDict();
    size_t i = 0
         , s = p->ChildSize();
    while( i < s ){
        // 添加数组项目
        string key = Conversion::String( Evaluat( p->Child(i) ) );
        if(key!=""){
            dict->Set( 
                key, 
                Evaluat( p->Child(i+1) )
            );
        }
        i += 2;
    }

    return dict;
}


/**
 * block 块数据结构建立
 */
DO* Exec::Block(Node*n)
{
    LOCALIZE_gc
    NodeBlock* p = (NodeBlock*)n;

    ObjectBlock* block = _gc->AllotBlock();
    size_t i = 0
         , s = p->ChildSize();
    while( i < s ){
        // 添加数组项目
        block->Push( _gc->AllotNode( p->Child(i) ) );
        i++;
    }

    return block;

}


/**
 * def 处理器定义
 */
DO* Exec::ProcDefine(Node* n)
{
    // cout<<"ProcDefine !!!"<<endl;
    NodeProcDefine* p = (NodeProcDefine*)n;

    ObjectProc* proc = new ObjectProc(p); //新建对象
    proc->stack = (void*)_envir._stack; // 定义所在栈帧环境

    string name = p->GetName();
    if(name!=""){ // 变量入栈
        // cout<<"VarPut(name, proc);"<<endl;
        VarPut(name, proc);
    }
    return proc;
}



/**
 * def 函数定义
 */
DO* Exec::FuncDefine(Node* n)
{
    LOCALIZE_gc
    // cout<<"FuncDefine !!!"<<endl;
    NodeFuncDefine* p = (NodeFuncDefine*)n;
    // 新建函数对象
    ObjectFunc* func = _gc->AllotFunc( p ); 
    // 处理默认参数
    ObjectDict* para = _gc->AllotDict(); 
    NodeGroup* pg =  (NodeGroup*)p->GetArgv();
    size_t len = pg->ChildSize();
    for(int i=0; i<len; i++){
        Node* li = pg->Child(i);
        if(li->type==NT::Assign){
            Node *nl = li->Left();
            if(nl->type==NT::Variable){
                DO* pv = Evaluat( li->Right() );
                para->Set( // 添加默认参数
                    nl->GetName(),
                    pv
                );
                _gc->Quote(pv); // 加引用
                // cout<<"default func parameter : "<<nl->GetName()<<endl;
                continue;
            } 
        }
        if(li->type!=NT::Variable){ // 参数格式错误
            ERR("Function parameter list format error !")
        }
        // cout<<"func parameter"<<endl;
    }
    func->argv = para; // 默认参数赋值
    func->stack = (void*)_envir._stack; // 定义所在栈帧环境
    // 变量入栈
    // cout<<"string name = p->GetName();"<<endl;
    string name = p->GetName();
    if(name!=""){
        VarPut(name, func);
    }
    return func;
}



/**
 * def 处理器调用
 */
DO* Exec::ProcCall(Node* n)
{
    LOCALIZE_gc
    // cout<<"ProcCall !!!"<<endl;
    NodeProcCall *p = (NodeProcCall*)n;
    // 获得处理器对象
    ObjectProc *op = (ObjectProc*)Evaluat( n->Left() );
    if(!op){
        ERR("Can't get the proc object !")
    }
    NodeProcDefine *proc = (NodeProcDefine*)op->GetNode();
    NodeGroup *pbody = (NodeGroup*)proc->GetBody();
    if(!pbody || !pbody->ChildSize()){
        return ObjNone(); //处理器体为空
    }
    // fbody->Print();
    // 拷贝环境
    Envir env = Envir(_envir);
    // 新栈帧
    Stack *stack = new Stack();
    stack->SetParent( (Stack*)op->GetStack() ); // 定义所在环境
    // 混合生成处理器参数
    BuildProcArgv(proc->GetArgv(), p->Right(), stack);
    // 调用执行环境对象入栈
    ObjectExec *oe = new ObjectExec(this);
    stack->VarPut("_call_", oe);
    // 环境更新
    env.Set(EnvirType::Proc);
    env.Set(stack);
    env.Set(pbody);
    // stack->Print();

    // 环境准备完毕，开始函数调用执行
    Exec exec = Exec(env);
    // 执行调用
    DO * retval = NULL;
    try
    {
        retval = exec.Run();
    }
    catch(Throw* tr) // 处理器返回
    {
        if(tr->GetType()!=ThrowType::Return){
            ERR("Function run excepction not <Return> !");
        }
        retval = tr->GetObject(); // 返回值
        delete tr;
    }
    // 处理器调用完成，清理执行栈
    map<string, DO*>::iterator itr_s = stack->v_local.begin();
    for(; itr_s != stack->v_local.end(); ++itr_s){
        _gc->Free( itr_s->second );
    }
    delete stack; //清除栈

    // 返回结果
    return retval;
}



/**
 * def 函数调用
 */
DO* Exec::FuncCall(Node* n)
{
    LOCALIZE_gc
    // cout<<"FuncCall !!!"<<endl;
    NodeFuncCall *p = (NodeFuncCall*)n;
    Node *lf = n->Left();
    Node *rt = p->Right();

    ObjectFunc *of;

    //调用变量自带函数
    DO *ofres = Objfunc(lf, rt);
    if(ofres){
        return ofres; // 调用成功
    }

    if(NT::Variable==lf->type){

        string name = lf->GetName();
        DO *obj = Variable(name);
        if(!obj){ // 未找到自定义变量，尝试调用系统函数
            DO *res = Sysfunc( name, rt );
            if(res){
                return res; //系统函数调用成功
            }
            ERR("Can't find the function \""+name+"\" !")
        }
        of = (ObjectFunc*)obj;

    }else{
        of = (ObjectFunc*)Evaluat( lf );
    }
    // 获得函数对象
    NodeFuncDefine *func = (NodeFuncDefine*)of->GetNode();
    NodeGroup *fbody = (NodeGroup*)func->GetBody();
    if(!fbody || !fbody->ChildSize()){
        return ObjNone(); //函数体为空
    }
    // fbody->Print();
    // 拷贝环境
    Envir env = Envir(_envir);
    // 新栈帧（父级栈帧）
    Stack *stack = new Stack();
    stack->SetParent( (Stack*)of->GetStack() );
    // 处理默认参数
    ObjectDict* para = of->argv;
    // cout<<"*para="<<(int)para<<endl;
    map<string, DO*>::iterator itr_p = para->value.begin();
    for(; itr_p != para->value.end(); ++itr_p){
        stack->VarPut( itr_p->first, itr_p->second );
        _gc->Quote(itr_p->second); // 加引用
    }
    // 混合生成处理器参数
    // cout<<"func->GetArgv();"<<endl;
    Node* ppp = func->GetArgv();
    // cout<<"Node* ppp="<<(int)ppp<<endl;
    BuildFuncArgv(func->GetArgv(), p->Right(), stack);
    // cout<<"BuildFuncArgv stack"<<endl;
    // stack->Print();
    // 环境更新
    env.Set(EnvirType::Func);
    env.Set(stack);
    env.Set(fbody);
    // stack->Print();

    // 环境准备完毕，开始函数调用执行
    Exec exec = Exec(env);
    // 执行调用
    DO * retval = NULL;
    try
    {
        retval = exec.Run();
    }
    catch(Throw* tr) // 函数返回
    {
        if(tr->GetType()!=ThrowType::Return){
            ERR("Function run excepction not <Return> !");
        }
        retval = tr->GetObject(); // 返回值
        delete tr;
    }
    // cout<<"bool done = exec.Run();"<<endl;
    // stack->Print();
    // 函数调用完成，清理执行栈
    map<string, DO*>::iterator itr_s = stack->v_local.begin();
    for(; itr_s != stack->v_local.end(); ++itr_s){
        _gc->Free( itr_s->second );
    }
    delete stack; //清除栈
    
    // 返回函数执行结果
    return retval;
}



/**
 * 函数返回
 */
DO* Exec::Return(Node*n)
{
    LOCALIZE_gc
    // cout<<"Return !!!"<<endl;
    NodeReturn *p = (NodeReturn*)n;
    // 求返回值
    DO* obj;
    Node* chd = p->Child();
    if(chd){
        obj = Evaluat( chd );
        _gc->Quote( obj ); // 加引用
    }else{
        obj = ObjNone(); // 默认返回 none
    }
    
    // 抛出返回
    throw new Throw(ThrowType::Return, "", obj);
}  





/**
 * 匹配处理器参数列表
 */
void Exec::BuildProcArgv(Node*form, Node*real, Stack*stack)
{
    LOCALIZE_gc

    size_t num_f = form ? form->ChildSize() : 0;
    size_t num_r = real ? real->ChildSize() : 0;
    size_t num_max = num_f > num_r ? num_f : num_r;
    // 参数列表
    ObjectList *argv = new ObjectList();
    //循环匹配参数
    for(int i=0; i<num_max; i++){
        DO* v = NULL;
        // 取值得到实参
        if(i<num_r){
            v = new ObjectNode( real->Child(i) );
            _gc->Quote( v ); // 加引用
            argv->Push( v );
        }
        if(!v){
            v = ObjNone(); // 无匹配 none
        }
        // 匹配形参并入栈
        if(i<num_f){
            Node *f = form->Child(i);
            if(f->type!=NT::Variable){
                ERR("Proc form parameter must be <Variable> type !")
            }
            string name = f->GetName();
            stack->VarPut(name, v);
            _gc->Quote( v ); // 加引用
        }
    }
    // 所有实参列表！
    stack->VarPut("_argv_", argv);

}

/**
 * 匹配函数参数列表
 */
void Exec::BuildFuncArgv(Node*form, Node*real, Stack*stack)
{
    LOCALIZE_gc

    // cout<<"BuildFuncArgv !!!"<<endl;
    size_t num_f = form ? form->ChildSize() : 0;
    size_t num_r = real ? real->ChildSize() : 0;
    size_t num_max = num_f > num_r ? num_f : num_r;
    // cout<<"num max="<<num_max<<", f="<<num_f<<", r="<<num_r<<endl;
    // 参数列表
    ObjectList *argv = _gc->AllotList();
    map<string, DO*> keypara; // 关键字参数
    //循环匹配参数
    for(int i=0; i<num_max; i++)
    {
        string name = ""; // 形式参数名
        DO* v = NULL; // 参数值
        bool iskp = false; // 是否为关键字参数
        // 取实参
        if(i<num_r){
            Node * li = real->Child(i);
            // 判断是否关键字参数
            if(li->type==NT::Assign){ 
                Node *n = li->Left();
                if(n->type==NT::Variable){
                    v = Evaluat( li->Right() );
                    keypara[n->GetName()] = v; //缓存关键字参数
                    iskp = true;
                }
            }
            if(!v){
                v = Evaluat( li );
            }
            argv->Push( v ); // 实参列表
            _gc->Quote(v);
        }
        // 取得形式参数名称
        if(i<num_f){ 
            Node *f = form->Child(i);
            if(f->type==NT::Assign){ //默认参数
                Node *nl = f->Left();
                if(nl->type==NT::Variable){
                    name = nl->GetName();
                } 
            }
            if(name==""){
                if(f->type!=NT::Variable){
                    ERR("Func form argv must be <Variable> type !")
                }
                name = f->GetName();
            }
        }
        // 形式参数匹配
        if(name!=""){
            DO *dft = stack->VarGet(name);
            if( iskp && !dft){ 
                //关键字参数项 没有默认值
                stack->VarPut(name, ObjNone());
            }
            // 正常匹配项
            if(!iskp && v){
                _gc->Quote(v); // 加引用
                stack->VarPut(name, v);
                if(dft) _gc->Free(dft);
            }
        }
    }
    // 关键字参数覆盖
    map<string, DO*>::iterator itr = keypara.begin();
    for(; itr != keypara.end(); ++itr){
        DO*old = stack->VarPut(itr->first, itr->second);
        if(old){
            _gc->Free(old); // 存在值则覆盖
        }
    }
    // 所有实参列表！
    stack->VarPut("_argv_", argv);

}






/**
 * ContainerAccess 容器访问
 */
DO* Exec::ContainerAccess(Node* n)
{
    // cout<<"-Exec::ContainerAccess-"<<endl;
    NodeContainerAccess* p = (NodeContainerAccess*)n;

    Node* con = p->Left();
    Node* idx = p->Right();
    size_t idxlen = idx->ChildSize();

    DO* result = NULL; //容器访问结果

    // cout<<"idxlen="<<idxlen<<endl;
    if(idxlen){ //索引个数

        DO* obj = Evaluat( con );
        OT ot = obj->type;
        DO* i1 = Evaluat( idx->Child(0) );
        OT i1t = i1->type;

        // 字典访问
        if(ot==OT::Dict){
            if(i1t!=OT::String){
                ERR("Dict key only <string> type !");
            }
            string key = Conversion::String( i1 );
            // cout<<"key = "<<key<<endl;
            result = ((ObjectDict*)obj)->Visit(key); 

        // 列表访问
        }else if(ot==OT::List){
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
        // 块访问
        }else if(ot==OT::Block){
            if(i1t!=OT::Int){
                ERR("Block index only <int> type !");
            }
            if(idxlen==1){ // 单个获取
                size_t i = Conversion::Long( i1 );
                if(!i){
                    ERR("Block index begin from <int> 1 !");
                }
                result = ((ObjectBlock*)obj)->Visit(i-1);
            }
        }
    }

    // cout<<"return result="<<(int)result<<endl;
    return result ? result : ObjNone(); // 无效访问 返回 none
}



/**
 * MumberAccess 成员访问
 */
DO* Exec::MemberAccess(Node* n)
{
    // cout<<"-Exec::MemberAccess-"<<endl;
    NodeMemberAccess *p = (NodeMemberAccess*) n;

    Node* left = p->Left();
    DO* base = Evaluat( left );
    Node* right = p->Right();
    if(right->type!=NT::Variable){
        ERR("Mumber name must be a Variable !");
    }
    // 访问名字
    string rname = right->GetName();

    return MemberAccess(base, rname);
}



/**
 * MumberAccess 成员访问
 */
DO* Exec::MemberAccess(DO* base, string name)
{
    DO* result = NULL; //成员访问结果
    OT bt = base->type;
    // 模块访问
    if( bt==OT::Module ){
        // cout<<"Module::key = "<<right->GetName()<<endl;
        result = ((ObjectModule*)base)->Visit( name ); 
    }

    if(!result){
        ERR("Can't find member '"+name+" !")
    }

    // cout<<"return result="<<(int)result<<endl;
    // return result ? result : ObjNone(); // 无效访问 返回 none
    return result;

}


/**
 * Import 模块加载
 */
DO* Exec::Import(Node* n)
{
    LOCALIZE_module
    LOCALIZE_stack
    LOCALIZE_gc

    // cout<<"-Exec::Import-"<<endl;
    DO* str = Evaluat( n->Child() );
    string name = Conversion::String( str );


    DO* mod; // 模块

    if(Module::IsSysmodName(name)){
        // 新建系统模块
        mod = new ObjectSysmod(name);
    }else{
        // 解析路径加载用户模块
        mod = Import( name );
    }

#ifdef WINDOWS
    // 替换字符
    // cout<<"ifdef WINDOWS : "<<name<<endl;
    Str::replace_all(name,"/","\\");
    // cout<<"endif WINDOWS : "<<name<<endl;
#endif

    if(mod){ //自动入栈
        string vn = Path::getName(name);
        if(Token::IsVariable(vn)){
            _stack->VarPut(vn, mod);
            _gc->Quote(mod);
        }
    }

    return mod;
}



/**
 * Import 模块加载
 */
DO* Exec::Import(string mdname)
{
	LOCALIZE_module

	// 加载模块
	ObjectModule* md;

    // 获得模块绝对路径
    string tarfile = Path::join( 
        Path::getDir(_envir._file),
        mdname
    );
    // cout<<"module path: "<<tarfile<<endl;

	string mdfile = Module::MatchFile(tarfile);
	if(mdfile==""){
		ERR("Can't find module\""+mdname+"\" !");
	}

	md = _module->GetCache( mdfile );
	if(md) return md; // 返回已经加载过的缓存的模块
	
	// 创建
	md = CreateModule(mdfile);

	if(!md){
		ERR("Can't create module\""+mdname+"\" !");
	}

	// 设置缓存
	_module->SetCache(mdfile, md);

	return md;
}

/**
 * 模块创建
 */
ObjectModule* Exec::CreateModule(string file)
{
    // cout<<"-Exec::CreateModule-"<<file<<endl;
	// 拷贝环境
	Envir le = Envir(_envir);

	le.SetFile( file );
	le.Set( EnvirType::Module );

	string text = Fs::ReadFile(file);
	Node *nd = Parse(text, file);
	// nd->Print();
    // return NULL; 
	le.Set( nd ); // 解析语法

	Stack *stack = new Stack(); // 新栈帧
	le.Set( stack );

    // cout<<"Exec nex = Exec(le);"<<endl;
	// 新建调用
	Exec nex = Exec(le);

    // cout<<"bool done = nex.Run();"<<endl;
    // 执行模块调用
    bool done = nex.Run();

    if(!done){
    	return NULL; // 执行失败
    }

    // cout<<"stack->Print();"<<endl;
    // stack->Print();

    // cout<<"ObjectModule *om = new ObjectModule();"<<endl;
    // 生成模块对象
    ObjectModule *om = new ObjectModule();
    map<string, DO*>::iterator itr = stack->v_local.begin();
    for(; itr != stack->v_local.end(); ++itr){
        om->Insert( itr->first, itr->second );
    }

    // cout<<"CreateModule!!!  = "<<om<<endl;
    return om;
}


/**
 * 调用系统函数
 * @name 函数名
 * @argv 参数列表
 */
DO* Exec::Sysfunc(string name, Node* para)
{
    // cout<<"-Exec::Sysfunc-"<<endl;
    LOCALIZE_gc
    LOCALIZE_stack

    NodeGroup* argv = (NodeGroup*) para;
    size_t len = argv->ChildSize();


    // 打印
    if(name=="print"){

        if(!len>0) return ObjNone();
        DO *obj = Evaluat( argv->Child(0) );
        DO::Print( obj );
        cout<<endl;
        return obj;

    // 求类型
    }else if(name=="type"){

        if(!len>0){
            ERR("System function \"call\" parameter size must 1 !")
        }
        DO *obj = Evaluat( argv->Child(0) );
        return _gc->AllotString( DO::GetTypeName(obj) );

    }else if(name=="int"){


    // 求容器大小
    }else if(name=="size"){
        size_t res_sz = 0;
        if(len>0){
            DO *obj = Evaluat( argv->Child(0) );
            OT t = obj->type;
            if(t==OT::String){ // 字符串长度
                res_sz = ((ObjectString*)obj)->value.size();
            }else if(t==OT::List){ // 列表大小
                 res_sz = ((ObjectList*)obj)->Size();
            }else if(t==OT::Dict){

            }
        }

        return _gc->AllotInt(res_sz);

    // 节点赋值 or 节点执行取值
    }else if(name=="assign" || name=="evaluat"){

        if(len>0){
            // 获得调用环境
            ObjectExec *oe = (ObjectExec*)_stack->VarGet("_call_");
            Exec *exec = (Exec*)oe->GetExec();
            // 节点对象
            ObjectNode *obj = (ObjectNode*)Evaluat( argv->Child(0) );
            OT t = obj->type;
            if(t!=OT::Node){ //错误
                ERR("System function evaluat parameter only type <Node> !")
            }
            Node *target = obj->GetNode();
            if(name=="evaluat"){
                // 在调用环境中执行节点对象
                DO* res = exec->Evaluat( target );
                if(!res){ //错误
                    ERR("System function evaluat call error !")
                }
                return res;
            }
            if(name=="assign"){
                if(!len>1){
                    ERR("System function assign parameter number must 2 !")
                }

                // 赋值
                DO* res = exec->Assign( target, Evaluat( argv->Child(1) ) );
                if(!res){ //错误
                    ERR("System function evaluat call error !")
                }
                return res;
            }
        }
    }

    // 系统函数查询失败
    return NULL;
}



/**
 * 调用变量自带函数
 * @name 函数名
 * @argv 参数列表
 */
DO* Exec::Objfunc(Node* left, Node* para)
{
    // 检测是否为变量自带函数调用
    if(NT::MemberAccess==left->type){

        DO *base = Evaluat( left->Left() );
        string name = left->Right()->GetName();
        return Objfunc(base, name, para);
    }

    return NULL;
}
DO* Exec::Objfunc(DO* base, string name, Node* para)
{
    // cout<<"-Exec::Objfunc-"<<endl;
    LOCALIZE_gc

    NodeGroup* argv = (NodeGroup*) para;
    size_t len = argv->ChildSize();

    OT bt = base->type;

    // 打印
    if(name=="print"){

        DO::Print( base );
        cout<<endl;
        return base;

    // 求类型
    }else if(name=="type"){


    }

    // 系统模块
    if(bt==OT::Sysmod){
        ObjectSysmod *mod = (ObjectSysmod*)base;
        string modname = mod->GetName();
        // 文件模块
        if(modname=="fs"){
            // 当前文件所在文件
            string path = Path::getDir(_envir._file);
            cout<<"Path::getDir(_envir._file) = "<<path<<endl;
            string p1 = ""; // 第一个参数
            if(len>0){
                DefObject* o1 = Evaluat( para->Child(0) );
                if(o1->type==OT::String){
                    p1 = ((ObjectString*)o1)->value;
                }
            }
            // 读取文件
            if(name=="read"){
                string tarfile = Path::join(path, p1);
                if(!Fs::Exist(tarfile)){
                    ERR("\""+tarfile+"\" is not find !")
                }
                // 读取文件
                return new ObjectString( Fs::ReadFile(tarfile) );
            }
        }


    }



    return NULL;
}



#undef LOCALIZE_module
#undef LOCALIZE_gc
#undef LOCALIZE_stack
#undef LOCALIZE_node



#undef DO   // DO
#undef OT   // ObjectType
#undef NT   // NodeType



} // end namespace vm
} // end namespace def



