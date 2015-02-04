
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>

#include "nodezer.h"

#define S Token::State
#define T TypeNode

using namespace std;

using namespace def::token;
using namespace def::node;

// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);

/**
 * 构造
 */
Nodezer::Nodezer(vector<Word>& w):
    words(w)
{
    Clear();
}



/**
 * 判断第一个参数（类型）是否为后面的类型任意之一
 */

bool Nodezer::IsType(T c,
    T t0=T::Null, T t1=T::Null, T t2=T::Null, T t3=T::Null, T t4=T::Null,
    T t5=T::Null, T t6=T::Null, T t7=T::Null, T t8=T::Null, T t9=T::Null)
{
    if(c==T::Null){ // 不可对比Null
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
void Nodezer::CurTypeNode()
{
    ctn = GetTypeNode(cur);
}


/**
 * 获得当前的节点类型
 */
TypeNode Nodezer::GetTypeNode(Word &cur)
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
        }

    }else if(s==S::Null){

        return T::Null; // 终止符
    }


    // 无匹配
    return T::Normal;

}


/**
 * 从当前单词新建节点
 */
Node* Nodezer::CreatNode(int mv=1, Node*l=NULL, Node*r=NULL)
{
    //Read();
    if(mv!=0) Move(mv); //移动指针

    Node *p = NULL;

    switch(ctn){

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

    case T::Add: // 加 +
        p = new NodeAdd(cur); break;
    case T::Sub: // 减 -
        p = new NodeSub(cur); break;
    case T::Mul: // 乘 *
        p = new NodeMul(cur); break;
    case T::Div: // 除 /
        p = new NodeDiv(cur); break;

    case T::Assign: // 赋值 =
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
Node* Nodezer::Express(Node *pp=NULL, T tt=T::Start)
{

// 值 叶节点
#define TN_VALUE T::Variable,T::None,T::Bool,T::Int,T::Float,T::String
#define TN_AS T::Add,T::Sub
#define TN_MD T::Mul,T::Div
#define TN_ASMD TN_AS,TN_MD

    //T t = T::Start; // 记录状态
    //string cv = cur.value; // 当前值
    Node *p = pp;
    T t = tt;

    while(t!=T::Down){

        Read();
        CurTypeNode(); // 当前类型
        T c = ctn;
        //cout<<(int)c<<"->"<<cur.value<<endl;

        //// Start
        if(t==T::Start){ //开始状态

            //cout << "-Start-" << endl;
            if( IsType(c,TN_VALUE) ){
                p = CreatNode(1);
                t = c;
            }else{
                return p;
            }

        //// Variable None Bool Int Float String
        }else if( IsType(t,TN_VALUE) ){

            //cout << "-TN_VALUE-" << endl;
            if( IsType(c,TN_VALUE) ){
                // 连续两个变量或值 表示表达式完毕
                return p;
            // 加 减 乘 除 
            }else if( IsType(c,TN_ASMD) ){
                p = CreatNode(1, p);
                t = c;
            // 赋值
            }else if( IsType(c,T::Assign) ){
                p = CreatNode(1, p);
                // 赋值算法后面的所有内容都将被赋值给左边的变量
                p->Right( Express() );
                return p;
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

        //// Null
        }else if(t==T::Null){ // 终止

            //cout << "-Null-" << endl;
            return p;

        }else{
            return p;
        }

    }

    return p;


#undef TN_VALUE
#undef TN_ASMD
#undef TN_AS
#undef TN_MD

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
 */
Node* Nodezer::Group()
{
    Node *node = new NodeGroup(cur);
    while(1){
        // 循环建立表达式
        Node *e = Express();
        if(e) node->AddChild( e );
        else break;
    }
    return node;
}


#undef S // Token::State
#undef T // TypeNode


/****************** 语法分析器测试 *******************/


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
    */
    
    
    cout << "\n\n";


    /* 打印词法分析结果
    for(int i=0; i<words.size(); i++){
    	Word wd = words[i];
		cout << wd.line << ","<< wd.posi << "  " << (int)wd.type << "  " << wd.value << endl;
    }
    */
    
    



    cout << "\n\n";


    /*

    Tokenizer TK("./Parser/test.d", true);
    vector <Word> words = TK.Scan();

    for(int i=0; i<words.size(); i++){
    	Word wd = words[i];
		cout << wd.line << ","<< wd.pos << "  " << (int)wd.type << "  " << wd.value << endl;
    }

    */

}




/**************** 语法分析器测试结束 *****************/