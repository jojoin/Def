/**
 * 抽象语法树解析抽象语法树解析
 */
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>

#include "nodezer.h"

using namespace std;


namespace def {
namespace parse {


#define S Token::State
#define T NodeType


// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);

/**
 * 构造
 */
Nodezer::Nodezer(vector<Word> *wds):
    words(wds)
{
    Clear();
}



/**
 * 判断第一个参数（类型）是否为后面的类型任意之一
 */
bool Nodezer::IsType(T c,
    T t0=T::End, T t1=T::End, T t2=T::End, T t3=T::End, T t4=T::End,
    T t5=T::End, T t6=T::End, T t7=T::End, T t8=T::End, T t9=T::End)
{
    if(c==T::End){ // 不可对比End
        return false;
    }
    if(c==t0||c==t1||c==t2||c==t3||c==t4||
       c==t5||c==t6||c==t7||c==t8||c==t9){
        return true;
    }else{
        return false;
    }
}



/**
 * 判断当前的节点类型
 */
NodeType Nodezer::CurNodeType()
{
    cnt = GetNodeType(cur);
    return cnt;
}


/**
 * 从当前单词获得当前的节点类型
 */
NodeType Nodezer::GetNodeType(Word &cwd)
{
    S s = cwd.type;
    string v = cwd.value;
    //S nt = next.type;
    //string nv = next.value;

    // cout<<"GetNodeType: "<<(int)s<<"->"<<v<<endl;

    if(s==S::Variable){
        return T::Variable; // 变量名

#define IF(name) }else if(s==S::name){ return T::name;
    IF(Int)  
    IF(Float)
    IF(String)
    IF(FuncCall) // 函数调用
    IF(ProcCall) // 处理器调用
    IF(ContainerAccess) // 容器访问
#undef IF

#define IF(str, name) }else if(v==str){ return T::name;

    }else if(s==S::Keyword){ // 关键字

        if(v==""){ return T::Normal;

        IF("none", None)
        IF("true", Bool)
        IF("false", Bool)
        IF("def", ProcDefine)
        IF("defun", FuncDefine)
        IF("class", ClassDefine)
        IF("print", Print)
        IF("if", If)
        IF("while", While)
        IF("import", Import) // 模块加载
        IF("return", Return) // 模块加载

        }

    }else if(s==S::Sign){ // 符号

        if(v==""){ return T::Normal;

        IF(":", Assign)         // 赋值 :
        IF("::", AssignUp)      // 向上查找赋值 ::
        IF(".", MemberAccess)   // 成员访问

        IF("(", List)      // 列表or优先级 (
        IF("[", Dict)      // 字典 [
        IF("{", Block)     // 块 {

        IF("+", Add)    // 加 +
        IF("-", Sub)    // 减 -
        IF("*", Mul)    // 乘 *
        IF("/", Div)    // 除 /

        IF("=", Equal)
        IF(">", More)
        IF("<", Less)
        IF(">=", MoreEqual)
        IF("<=", LessEqual)
        IF("~=", NotEqual)
        IF("~", Not)

        }

    }else if(s==S::End){

        return T::End; // 终止符
    }


    // 无匹配
    return T::Normal;

#undef IF


}



/**
 * 取得节点的优先级
 */
int Nodezer::GetPriority(Node*p)
{
    if(p==NULL){
        return 0;
    }

    switch(p->type){

#define N(name, priori) 
#define D(name, priori) case T::name: return priori;
    NODELIST(N, D)
#undef N
#undef D

    }

    return 0;
}



// 节点类型定义
#define TN_VALUE T::Variable,T::None,T::Bool,T::Int,T::Float,T::String
#define TN_AS T::Add,T::Sub
#define TN_MD T::Mul,T::Div
#define TN_ASMD TN_AS,TN_MD
#define IS_SIGN(str) cur.type==S::Sign&&cur.value==str
#define IS_NO_SIGN(str) cur.type!=S::Sign||cur.value!=str
#define IS_KEYWORD(str) cur.type==S::Keyword&&cur.value==str
#define IS_NO_KEYWORD(str) cur.type!=S::Keyword||cur.value!=str




/**
 * 从当前单词新建节点
 */
Node* Nodezer::CreatNode()
{
    //Read();
    T cnt = CurNodeType();
    //Move(1);

    // cout<<"CreatNode: "<<(int)cnt<<"->"<<cur.value<<endl;

    switch(cnt){

#define N(name, priori) 
#define D(name, priori) case T::name: return new Node##name(cur);
    NODELIST(N, D)
#undef N
#undef D


    }

    return NULL;
}


/**
 * 解析延展部分节点
 */
Node* Nodezer::ParseNode(Node*p1, Node*p)
{
    if(p==NULL){ 
        return NULL; //表达式完成
    }

    T t = p->type;

    // 列表 or 优先级 or 函数调用
    if( t==T::List ){
        // cout << "-List or Priority or FuncCall-" << endl;
        if(p1 && p1->type==T::FuncCall){
            delete p;
            Move(1); //jump (
            Node* g = Group(); 
            Move(1); //jump )
            return g; // 返回函数调用右节点
        }
        // 列表or优先级
        NodePriority* pp = new NodePriority(cur);
        Move(1); //jump (
        Node *e = Express();
        if(e && IS_SIGN(")")){ //优先级
            delete p;
            if(!GetPriority(e)){
                // 不影响优先级计算
                cout<<"!GetPriority(e)"<<endl;
                delete pp;
                Move(1); // jump )
                return e;
            }
            pp->AddChild(e);
            Move(1); // jump )
            return pp;
        }
        if(e) p->AddChild(e);
        while(1){
            if(IS_SIGN(",")){
                Move(1); continue;
            }
            if(IS_SIGN(")")){
                Move(1); // jump )
                break; //列表结束
            }
            Node *e = Express();
            if(!e){
                ERR("Err: list didn't end with ) sign !");
            }
            p->AddChild(e);
        }
        //delete pp;
        return p; // 列表结束

    // 字典 or 容器访问
    }else if( t==T::Dict ){
        // cout << "-Dict or ContainerAccess-" << endl;
        Move(1); // jump [
        if(p1 && p1->type==T::ContainerAccess){
            // cout << "Parse []" << endl;
            delete p;
            Node *g = p = Group();
            Move(1); //jump ]
            return g; // 返回容器访问右节点
        }
        // 字典
        while(1){
            if(IS_SIGN("]")){
                break; //字典结束
            }
            Node *e = Express();
            if(!e){
                ERR("Err: dict parse , IS_SIGN no ]");
            }
            p->AddChild(e);
        }
        Move(1); //jump ]
        return p;
        
    /* 容器访问  函数调用  处理器调用
    }else if( t==T::ContainerAccess || t==T::FuncCall || t==T::ProcCall){
        cout << "-ContainerAccess FuncCall ProcCall-" << endl;
    */

    // 块 or 处理器调用
    }else if( t==T::Block ){
        //cout << "-Block or ProcCall-" << endl;
        Move(1); //jump {
        if(p1 && p1->type==T::ProcCall){
            delete p;
            Node* g = Group(); 
            Move(1); //jump }
            return g; // 返回处理器调用右节点
        }
        // 块
        while(1){
            if(IS_SIGN("}")){
                break; // 块结束
            }
            Node *e = Express();
            if(!e){
                ERR("Err: block parse , IS_SIGN no }");
            }
            p->AddChild(e);
        }
        Move(1); //jump }
        return p;

    // 函数 处理器 类 定义
    }else if( t==T::FuncDefine || t==T::ProcDefine || t==T::ClassDefine ){
        // cout << "-FuncDefine ProcDefine ClassDefine-" << endl;
        bool pd = t==T::ProcDefine;
        Move(1); // jump def/defun/class
        if(cur.type==S::Variable){
            string name = cur.value;
            Move(1); // jump name
            bool tpc = cur.type==S::ProcCall;
            if( pd&&tpc || !pd&&cur.type==S::FuncCall){
                p->SetName( name ); //名称
                Move(1); // jump ...Call
            }else{
                Move(-1); //复位
            }
        }
        if( pd&&IS_SIGN("{") || !pd&&IS_SIGN("(") ){ // 参数
            Move(1); // jump ( or {
            p->SetArgv( Group() );
            Move(1); // jump ) or }
        }
        p->SetBody( Group() );
        Move(1); // jump ;
        return p;

    // 条件分支
    }else if( t==T::If ){

        //cout << "-If-" << endl;
        Move(1); // jump if
        // 开始构建 If 块结构
        Node *gr = new NodeGroup(cur);
        while(1){

            if(IS_KEYWORD("elif")){
                p->AddChild(gr);
                gr = new NodeGroup(cur);
                Move(1); // jump elif
                continue;
            }else if(IS_KEYWORD("else")){
                p->AddChild(gr);
                gr = new NodeGroup(cur);
                gr->AddChild(NULL); //第一个子节点为空节点，表示为 else 节点
                Move(1); // jump else
                continue;
            }else if(IS_SIGN(";")){
                p->AddChild(gr);
                gr = new NodeGroup(cur);
                Move(1); // jump ;
                break; //If结构结束
            }
            //添加表达式
            gr->AddChild( Express() );

        }//end while
        return p;

    // 循环
    }else if( t==T::While ){

        //cout << "-While-" << endl;
        //Node *wh = new NodeWhile(cur);
        Move(1); // jump while
        while(1){
            if(IS_SIGN(";")){
                break; //If结构结束
            }
            //添加表达式
            p->AddChild( Express() );
        }//end while
        Move(1); // jump ;
        return p;

    // 模块加载 打印 函数返回
    }else if( t==T::Import || t==T::Print || t==T::Return ){

        // cout << "-Import Print Return-" << endl;
        Move(1); // jump import print return
        p->AddChild( Express() );
        return p;

    }

    // 当前节点不需要扩展 跳到下一个
    Move(1); //下一个
    return p;
}




/**
 * 按优先级组合语法节点
 * down 是否为上级运算组合下降
 */
Node* Nodezer::AssembleNode(Node*p1, Node*p2, bool down=false)
{

    int s1 = GetPriority(p1);
    int s2 = GetPriority(p2);

    // cout<<"s1="<<s1<<", s2="<<s2<<endl;
    
    if(down && s1 && s2 && s2<=s1){ //上级下降
        //cout << " down && s1 && s2 && s2<s1 " << endl;
        return NULL; //下降完成返回

    }else if(s1 && s2 && s2<=s1){ // 左结合
        //cout << " s1 && s2 && s2<=s1 " << endl;
        p2->Left( p1 );
        return p2;

    }else if( s1 && s2>s1 ){ // 优先级调整
        //cout << " s1 && s2>s1 " << endl;
        Node *p1r = p1->Right();
        if(!p1r&&p2->type!=T::Sub){
            ERR("ERR: !p1r ");
        }
        p2->Left( p1r );
        Move(1); // 下一个参与组合
        p1->Right( Express( p2, true ) ); //递归下降
        Move(-1); // 下降返回后需要回退，以便于上级节点重新组合 
        return p1;

    }else if( !s1 && s2 ){ // 左叶
        //cout << " !s1 && s2 " << endl;
        p2->Left( p1 );
        return p2;

    }else if( s1 && !s2 ){ // 右叶
        //cout << " s1 && !s2 " << endl;
        Node *p1r = p1->Right();
        if(p1r){
            return NULL; // 表达式完成
        }
        p1->Right( p2 );
        return p1;
    }

    // 无法组装节点  表达式完成
    return NULL;

}





/**
 * 建立语法表达式
 */
Node* Nodezer::Express(Node *p1, bool down)
{
    while(1){

        Node *p2;
        if(nodebuf){ // 获取上一步缓存节点
            p2 = nodebuf; nodebuf = NULL;
        }else{
            p2 = CreatNode(); //新建
        }
        if(cur.type==S::End){ // 全部结束
            return p2 ? p2 : p1;
        }
        if(!p2){ //表达式完成
            return p1;
        }
        if(!p1){
            p1 = ParseNode(p1, p2); //延展完善
            continue; // 获取下一个
        }

        // cout<<"p1="<<(int)p1->type<<", p2="<<(int)p2->type<<endl;

        // 按优先级组合节点
        Node* pn = AssembleNode(p1, p2, down);
        //cout << "  Node* pn =  " <<pn<< endl;
        if(pn){ // 可以组合
            p2 = ParseNode(p1, p2); // 延展完善
            if( pn->Right() ){
                pn->Right( p2 ); // 重置完善后的右叶（修复BUG！）
            }
            p1 = pn;
            continue; //优先级组合成功，下一步组合
        }

        // 节点无法组合
        //cout <<"DeleteNode:"<<endl;
        nodebuf = p2; //缓存新建的节点，下一步重新开始
        return p1;

    }



}



/**
 * 构建语法树
 */
Node* Nodezer::BuildAST()
{
    Read();
    return Group();
}


/**
 * 表达式组合
 */
Node* Nodezer::Group()
{
    NodeGroup* gr = new NodeGroup(cur);

    while(1){
        if(IS_SIGN(",")){
            Move(1); //跳过
            continue;
        }
        if(IsGroupEnd()){ // 结束符号
            break;
        }
        // 循环建立表达式
        Node *e = Express();
        // cout << "-Nodezer::Group while-" << endl;
        if(!e){
            break;
        }
        //添加
        gr->AddChild( e );
    }
    return gr;
}


/**
 * 表达式组合完结
 * @param endl 要检测的结束符号 ) ] } ; 
 */
bool Nodezer::IsGroupEnd()
{
    if(cur.type==S::Sign){
        string v = cur.value;
        if( v==")" ||
            v=="]" ||
            v=="}" ||
            v==";"
        ){
            //cout<<"end by )]}; ＝ "<<v<<endl;
            return true; // 表达式组结束
        }
    }

    return false;

}



#undef TN_VALUE
#undef TN_ASMD
#undef TN_AS
#undef TN_MD
#undef IS_SIGN
#undef IS_NO_SIGN
#undef IS_KEYWORD
#undef IS_NO_KEYWORD





#undef S // Token::State
#undef T // NodeType



#undef NODELIST


} // --end-- namespace parse
} // --end-- namespace def
