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
Nodezer::Nodezer(vector<Word>& w, string file=""):
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
 * 获得当前的节点类型
 */
NodeType Nodezer::GetNodeType(Word &cur)
{
    S s = cur.type;
    string v = cur.value;
    //S nt = next.type;
    //string nv = next.value;

    //cout<<(int)s<<"->"<<v<<endl;

    if(s==S::Symbol){

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

    }else if(s==S::End){

        return T::End; // 终止符
    }


    // 无匹配
    return T::Normal;

}


/**
 * 从当前单词新建节点
 */
Node* Nodezer::CreatNode(int mv=0, Node*l=NULL, Node*r=NULL)
{
    //Read();
    if(mv!=0) Move(mv); //移动指针

    Node *p = NULL;

    //cout<<(int)cnt<<"->"<<cur.value<<endl;

    switch(cnt){

    case T::Variable: // 变量
        return new NodeVariable(cur);
    case T::None:     // None
        return new NodeNone(cur);
    case T::Bool:     // Bool
        return new NodeBool(cur);
    case T::Int:      // Int
        return new NodeInt(cur);
    case T::Float:    // Float
        return new NodeFloat(cur);
    case T::String:   // String
        return new NodeString(cur);

    case T::Print: // 打印
        return new NodePrint(cur);

    case T::If: // if 控制结构
        return new NodeIf(cur);

    case T::While: // while 循环控制
        p = new NodeWhile(cur); break;

    case T::Add: // 加 +
        p = new NodeAdd(cur); break;
    case T::Sub: // 减 -
        p = new NodeSub(cur); break;
    case T::Mul: // 乘 *
        p = new NodeMul(cur); break;
    case T::Div: // 除 /
        p = new NodeDiv(cur); break;

    case T::Assign: // 赋值 =
        //ERR("new NodeAssign(cur)")
        p = new NodeAssign(cur); break;
    }

    if(p&&l) p->Left(l);
    if(p&&r) p->Right(r);

    return p;
}


/**
 * 扫描单词 构建单条表达式
 * @param pp 上级递归父节点
 * @param tt 上级递归父节点类型
 */
Node* Nodezer::Express(Node *pp=NULL, T tt=T::Normal)
{

// 值 叶节点
#define TN_VALUE T::Variable,T::None,T::Bool,T::Int,T::Float,T::String
#define TN_AS T::Add,T::Sub
#define TN_MD T::Mul,T::Div
#define TN_ASMD TN_AS,TN_MD
#define IS_SIGN(str) cur.type==S::Sign&&cur.value==str
#define IS_KEYWORD(str) cur.type==S::Keyword&&cur.value==str
#define ELIF_CTN }else if(IsType(c,T)){ t=c; continue;

    //T t = T::Normal; // 记录状态
    //string cv = cur.value; // 当前值
    Node *p = pp;
    T t = tt;
    T c = cnt;
    size_t old_i = 0;


    while(t!=T::End){

        if(i==0||old_i!=i){ // 是否重新读取token
            Read();
            c = CurNodeType(); // 当前类型
            //cout<<(int)c<<"->"<<cur.value<<endl;
            old_i = i;
        }else{
            //cout<<(int)c<<"->"<<cur.value<<endl;
            t = c;
        }

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

            // () 括号优先级
            }else if(IS_SIGN("(")){
                Move(1); //跳过左括号
                size_t num = 0;
                while(1){
                    if(num>0){

                    }
                    num++;
                }
                p = Express(); //一条表达式
                if(IS_SIGN(")")) ERR("err: )"); //错误处理
                Move(1); //跳过右括号
                return p;
            }else{
                return p;
            }

        //// Variable None Bool Int Float String
        }else if( IsType(t,TN_VALUE) ){

            // cout << "-TN_VALUE-" << endl;
            /*if( IsType(c,TN_VALUE,T::Print) ){
                // 连续两个变量或值 表示表达式完毕
                return p;
            // 加 减 乘 除 
            }else */
            if( IsType(c,TN_ASMD) ){
                p = CreatNode(1, p);
                t = c;
            // 变量赋值
            }else if( IsType(t,T::Variable) && IsType(c,T::Assign) ){
                continue;
            }else{ 
                return p;
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

#undef TN_VALUE
#undef TN_ASMD
#undef TN_AS
#undef TN_MD
#undef IS_SIGN
    
    return p;

}



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
    Node *node = new NodeGroup(cur);
     
    while(1){
        // 循环建立表达式
        Node *e = Express();
        // cout << "-Nodezer::Group while-" << endl;
        if(e) node->AddChild( e );
        else break;
    }
    return node;
}


#undef S // Token::State
#undef T // NodeType


/****************** 语法分析器测试 *******************


int main()
{
    cout << "\n";


    // 词法分析结果
    vector<Word> words;

    // 初始化词法分析器
    Tokenizer T(true, "./test.d", words);

    // 执行词法分析
    T.Scan();

    // 初始化语法分析器
    Nodezer N(words);

    // 解析得到语法树（表达式）
    Node *node = N.BuildAST();

    cout << "\n\n";

    //delete node;

    /*
    cout <<
    node->Child(0)->Right()->Left()->GetFloat()
    << endl;
    *
    
    
    cout << "\n\n";


    /* 打印词法分析结果
    for(int i=0; i<words.size(); i++){
    	Word wd = words[i];
		cout << wd.line << ","<< wd.posi << "  " << (int)wd.type << "  " << wd.value << endl;
    }
    *
    
    



    cout << "\n\n";


    /*

    Tokenizer TK("./Parser/test.d", true);
    vector <Word> words = TK.Scan();

    for(int i=0; i<words.size(); i++){
    	Word wd = words[i];
		cout << wd.line << ","<< wd.pos << "  " << (int)wd.type << "  " << wd.value << endl;
    }

    *

}




**************** 语法分析器测试结束 *****************/