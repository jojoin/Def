/**
 * 抽象语法树解析抽象语法树解析
 */
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>

#include "nodezer.h"

#define S Token::State
#define T NodeType

using namespace std;

using namespace def::token;
using namespace def::node;

// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);

/**
 * 构造
 */
Nodezer::Nodezer(vector<Word>*w, string file=""):
    words(w),
    filepath(file)
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

    /* }else if(s==S::None){
        return T::None;
    }else if(s==S::Bool){
        return T::Bool; */
    }else if(s==S::Int){
        return T::Int;
    }else if(s==S::Float){
        return T::Float;
    }else if(s==S::String){
        return T::String;

    }else if(s==S::Keyword){ // 关键字

        if(v=="none"){
            return T::None;
        }else if(v=="true"||v=="false"){
            return T::Bool;

        }else if(v=="print"){
            return T::Print;
        }else if(v=="if"){
            return T::If;
        }else if(v=="while"){
            return T::While;
        }

    }else if(s==S::Sign){ // 符号

        if(v=="="){
            return T::Assign; //赋值 =

        }else if(v=="+"){
            return T::Add; // 加 +
        }else if(v=="-"){
            return T::Sub; // 减 -
        }else if(v=="*"){
            return T::Mul; // 乘 *
        }else if(v=="/"){
            return T::Div; // 除 /

        /*}else if(v=="("){
            return T::Priority; // 优先级 ()
        **/
        }

    }else if(s==S::JoinWith){

        Move(1); Read();
        if(cur.type==S::Variable){
            Move(1); Read();
            if(cur.type==S::Sign){
                string v = cur.value;
                if(v=="["){
                    Move(-1); Read();
                    return T::ContainerAccess; // 容器访问
                }else if(v=="("){
                    Move(-1); Read();
                    return T::FuncCall;     // 函数调用
                }
            }
        }
        return CurNodeType();

    }else if(s==S::End){

        return T::End; // 终止符
    }


    // 无匹配
    return T::Normal;

}



/**
 * 返回节点的优先级
 */
int Nodezer::GetPriority(Node*p)
{
    if(p==NULL){
        return 0;
    }

    switch(p->type){

#define N(name, priority) 
#define D(name, priority) case T::name: return priority;
    NODELIST(N, D)
#undef N
#undef D

    }

    return 0;
}



// 值 叶节点
#define TN_VALUE T::Variable,T::None,T::Bool,T::Int,T::Float,T::String
#define TN_AS T::Add,T::Sub
#define TN_MD T::Mul,T::Div
#define TN_ASMD TN_AS,TN_MD
#define IS_SIGN(str) cur.type==S::Sign&&cur.value==str
#define IS_KEYWORD(str) cur.type==S::Keyword&&cur.value==str
#define ELIF_CTN }else if(IsType(c,T)){ t=c; continue;





/**
 * 从当前单词新建节点
 */
Node* Nodezer::CreatNode()
{
    Read();
    T cnt = CurNodeType();
    Move(1);

    cout<<"CreatNode: "<<(int)cnt<<"->"<<cur.value<<endl;

    switch(cnt){

#define N(name, priority) 
#define D(name, priority) case T::name: return new Node##name(cur);
    NODELIST(N, D)
#undef N
#undef D


    }

    return NULL;
}


/**
 * 解析并建立节点
 */
Node* Nodezer::ParseNode(Node*op=NULL, Node*p=NULL)
{
    if(p==NULL){ 
        return NULL;
    }

    T t = p->type;

    // ContainerAccess
    if( t==T::ContainerAccess ){

        return p;

    // FuncCall
    }else if( t==T::FuncCall ){

        // cout << "-FuncCall-" << endl;
        Move(1); // 跳过括号
        Node* list = Group();  
        cout << "-Node* list = Group();-" << endl;
        p->Right( list ); // 参数列表
        //Move(1); // 跳过括号
        return p;
    }

    // cout << "-ParseNode  p->type  nothing-" << endl;


    return p;
}




/**
 * 按优先级组合表达式
 */
Node* Nodezer::AssembleNode(Node*p1, Node*p2)
{

    int s1 = GetPriority(p1);
    int s2 = GetPriority(p2);

    //cout<<"s1="<<s1<<", s2="<<s2<<endl;

    if( s1 && s2>s1 ){

        //cout << " p1->Right( Express( p2 ) ) " << endl;
        Node *p1r = p1->Right();
        if(!p1r){
            ERR("ERR: !p1r ");
        }
        p2->Left( p1r );
        p1->Right( Express( p2 ) ); //递归下降
        return p1;

    }else if( s2 ){ // 左叶

        //cout << " p2->Left( p1 ) " << endl;
        p2->Left( p1 );
        return p2;

    }else if( s1 && !s2 ){ // 右叶

        //cout << " p1->Right( p1r ) " << endl;
        Node *p1r = p1->Right();
        if(p1r){
            return NULL; //表达式完成
        }
        p1->Right( p2 );
        return p1;
    }


    return NULL;

}



/**
 * 创建语法表达式节点
 */
Node* Nodezer::Express(Node *p1=NULL)
{
    Node *p2 = NULL;

    while(1){

        if(cnode){ // 缓存
            p2 = cnode; cnode=NULL;
        }else{ // 延展
            p2 = ParseNode( p1, CreatNode() );
        }

        if(!p2){ // 表达式结束
            // Move(1);Read(); // 回退一个词
            return p1;
        }

        if(!p1){
            p1 = p2;
            continue;
        }

        //cout<<"p1="<<(int)p1->type<<", p2="<<(int)p2->type<<endl;

        Node* pn = AssembleNode(p1, p2);
        //cout << "  Node* pn =  " <<pn<< endl;
        if(pn){
            p1 = pn;
            // cout << " p1 = pn;" << endl;
            continue; //优先级组合成功
        }

        // 查找无法组合原因
        int s1 = GetPriority(p1);
        int s2 = GetPriority(p2);

        if( !s2 ){ // 表达式完成
            //cout << " cnode = p2; " << endl;
            cnode = p2;
            return p1;
        }

        p1 = p2;
    }



}



/**
 * 扫描单词 构建单条表达式
 * @param pp 上级递归父节点
 * @param tt 上级递归父节点类型
 *
Node* Nodezer::Express(Node *pp=NULL, T tt=T::Normal)
{

    //T t = T::Normal; // 记录状态
    //string cv = cur.value; // 当前值
    Node *p = pp;
    T t = tt;
    T c = cnt;
    size_t old_i = 0;


    while(t!=T::End){

        if(i==0||old_i!=i){ // 已经move至下一个，重新读取词
            Read();
            c = CurNodeType(); // 当前类型
            //cout<<(int)c<<"->"<<cur.value<<endl;
            old_i = i;
        }else{ // 未 move，无需重新读取
            t = c;
        }

        // cout<<(int)c<<"->"<<cur.value<<endl;

        // 正式开始循环处理

        //// Normal
        if(t==T::Normal){ //默认状态

            //cout << "-Normal-" << endl;
            if( IsType(c,T::Print,T::If,T::While) ){
                //cout << "Normal continue !" << endl;
                t = c;
                continue;

            }else if( IsType(c,TN_VALUE) ){
                p = CreatNode(1);
                t = c;

            // [] 列表数据结构
            }else if(IS_SIGN("[")){
                //cout << "[] continue !" << endl;
                c = T::List;
                continue;

            }else{
                return p;
            }

        //// Variable None Bool Int Float String
        }else if( IsType(t,TN_VALUE) ){

            // cout << "-TN_VALUE-" << endl;

            if( t==T::Variable && c==T::ContainerAccess ){ // 容器访问 []
                p = CreatNode(1, p);
                Node *r = Express(); // 一条表达式 作为访问索引
                if(IS_SIGN("]")) ERR("err: ]"); //错误处理
                Move(1); //跳过右方括号 ]
                p->Right(r);
            }else if( t==T::Variable && c==T::FuncCall ){ // 函数调用 ()

                string name = p->GetName();
                p = CreatNode(1); // 右节点参数列表
                p->SetName(name); // 设置函数名称
                Node* paralist = Group();
                p->Right(paralist); // 设置参数列表
            
            }else if( t==T::Variable && c==T::Assign ){ // 变量赋值
                continue;

            // 运算
            }else if( IsType(c,TN_ASMD) ){
                p = CreatNode(1, p);
                t = c;
            }else{

                // cout << "}else{" << endl;
                // cout << cur.value << endl;
                return p; // 表达式结束
            }


        //// Add Sub
        }else if( IsType(t,TN_AS) ){ // 加法 减法 + -

            //cout << "-Add,Sub-" << endl;
            if( IsType(c,TN_VALUE) ){
                if(p->Right()){
                    //已经存在右节点
                    return p;
                }
                p->Right(CreatNode(1));
            // 同级左结合算符 + -
            }else if( IsType(c,TN_AS) ){
                p = CreatNode(1, p);
                t = c;
            // 优先算符 * /
            }else if( IsType(c,TN_MD) ){
                Node *pn = CreatNode(1);
                Node *r = p->Right();
                pn->Left(r);
                pn = Express(pn, c);
                p->Right(pn);
            }else{ 
                return p;
            }

        //// Mul Div
        }else if( IsType(t,TN_MD) ){ // 乘法 除法 * /

            //cout << "-Mul,Div-" << endl;
            if( IsType(c,TN_VALUE) ){
                if(p->Right()){
                    //已经存在右节点
                    return p;
                }
                p->Right(CreatNode(1));
            // 同级左结合算符
            }else if( IsType(c,TN_MD) || 
                //优先级低的算符且不是上一层递归传入
                !pp && IsType(c,TN_AS)
            ){
                p = CreatNode(1, p);
                t = c;
            }else{
                return p;
            }


        // FuncCall
        }else if( t==T::FuncCall ){

            //cout << "-FuncCall-" << endl;
            NodeFuncCall* fp = CreatNode(1);
            Move(1); // 跳过左括号 (
            fp->Right( Group() ); // 设置参数列表
            if(IS_SIGN(")")) ERR("err: )"); //错误处理
            if(p!=NULL){ // 表达式
                p->Right(fp);
                t ＝ p->type; // 恢复状态
            }else{
                return fp;
            }

        // Assign
        }else if( t==T::Assign ){

            //cout << "-Assign-" << endl;
            p = CreatNode(1, p);
            // 赋值算法后面的所有内容都将被赋值给左边的变量
            p->Right( Express() );
            return p;

        // While
        }else if( t==T::While ){

            //cout << "-While-" << endl;
            p = CreatNode(1);
            NodeGroup* g = new NodeGroup(cur);
            while(1){
                if(IS_SIGN(";")){
                    size_t s = g->ChildSize();
                    Node *nl = NULL
                        ,*nr = NULL;
                    if(s==0){
                    }else if(s==1){
                        nl = g->Child(0); // 第一个表达式作为 while 条件
                    }else if(s>=2){
                        nl = g->ChildPop(0);
                        nr = g;
                    }
                    p->Left(nl);
                    p->Right(nr);
                    Move(1);
                    break;
                }
                g->AddChild( Express() );
            }
            return p;

        // If
        }else if( t==T::If ){

            //cout << "-If-" << endl;
            p = CreatNode(1);
            NodeGroup* g = new NodeGroup(cur);
            string evt = "if";
            // 开始构建 If 块结构
            while(1){
                bool end = IS_SIGN(";")
                   , elif = IS_KEYWORD("elif")
                   , el = IS_KEYWORD("else");
                if(end||elif||el){ // 写入上部流程块
                    size_t s = g->ChildSize();
                    if(evt=="if"){
                        if(s==0){
                            p->AddChild(NULL);
                            p->AddChild(NULL);
                        }else if(s==1){
                            p->AddChild(g->Child(0)); // 第一个表达式作为if条件
                            p->AddChild(NULL);
                        }else if(s>=2){
                            p->AddChild(g->ChildPop(0));
                            p->AddChild(g);
                        }
                    }else if(evt=="else"){
                        p->AddChild(g);
                    }
                    Move(1);
                    g = new NodeGroup(cur);
                }
                if(end){ // If 结束
                    break;
                }
                if(el){
                    evt = "else"; //块改变
                }
                g->AddChild( Express() );
            }
            return p;

        // List 列表结构
        }else if( t==T::List ){

            //cout << "-List-" << endl;
            Word start = cur;
            Move(1); //跳过 [
            NodeList *list = new NodeList(cur);
            while(1){ // 列表项
                Node *e = Express();
                if(e) list->AddChild( e );
                else break;
                //cout<<"list item"<<endl;
            }
            if(IS_SIGN("]")){
                //cout<<"IS_SIGN(])"<<endl;
                Move(1); //跳过 ]
                //cout<<"Move(1)"<<endl;
                return list; //成功返回列表结构
            }else{
                //cout<<"Error(301)"<<endl;
                Error(301, start); //缺少右括号匹配
            }

        // Print
        }else if( t==T::Print ){

            // cout << "-Print-" << endl;
            p = CreatNode(1);
            // print 关键字左边的第一个值将被打印
            p->Right( Express() );
            return p;

        //// End 终止
        }else if( t==T::End ){

            //cout << "-End-" << endl;
            return p;

        }else{
            return p;
        }

    }

    return p;

}
*/



/**
 * 构建语法树
 */
Node* Nodezer::BuildAST()
{
    return Group();
}


/**
 * 表达式组合
 * @param t 组合类型
 */
Node* Nodezer::Group()
{
    NodeGroup *node = new NodeGroup(cur);
     
    while(1){
        // 循环建立表达式
        Node *e = Express();
        // cout << "-Nodezer::Group while-" << endl;
        if(e){
            node->AddChild( e );
            if(IsGroupEnd()){
                return node;
            }
        }else{
            break;
        }
    }
    return node;
}


/**
 * 表达式组合完结
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
            cout<<")]};"<<endl;
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
    




#undef S // Token::State
#undef T // NodeType



#undef NODELIST

