
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>

#include "nodezer.h"
#include "../Util/str.h"

using namespace std;

using namespace def::token;
using namespace def::node;
using namespace def::util;


/**
 * 构造
 */
Nodezer::Nodezer(vector<Word>& w):
    words(w)
{
    Clear();
}



#define S Token::State
#define T TypeNode


/**
 * 扫描单词 构建语法树
 * @param tn 期望return的表达式类型
 */

Node* Nodezer::Scan()
{
    //TypeNode ctn = tn_stk.back();
    //tn_stk.pop_back();//移除

    if(ctn==T::Expression){

        //cout<<"-Expression-"<<endl;
        NodeExpression *node = new NodeExpression(cur.line, cur.posi);
        while(1){
            Read();
            if( //退出条件
                cur.type==nullword.type ||
                cur.type==S::Sign&&cur.value==";" ||
                cur.type==S::Sign&&cur.value=="}"
            ) break;
            Judge();//预判
            node->AddChild( Scan() );
        }
        return node;

    }else if(ctn==T::Assignment){ // = 赋值语句

        //cout<<"-Assignment-"<<endl;
        NodeAssignment *node = new NodeAssignment(next.line, next.posi);
        node->LeftChild(new NodeVariable(cur.line, cur.posi, cur.value));
        Jump(2);
        Judge();//预判
        node->RightChild( Scan() );
        return node;

    }else if(ctn==T::Variable){ // 变量

        //cout<<"-Variable-"<<endl;
        Jump();
        return new NodeVariable(cur.line, cur.posi, cur.value);

    }else if(ctn==T::Int){ // int 值
        
        //cout<<"-Int-"<<endl;
        Jump();
        return new NodeInt(cur.line, cur.posi, Str::s2l(cur.value));
    }




    //cout<<"  ############# "<<endl;

}


/**
 * 执行预判
 */
void Nodezer::Judge()
{
    //tn_stk.push_back( JudgeTypeNode() );
    ctn = JudgeTypeNode();
}

/**
 * 预判当前的 节点类型
 */
TypeNode Nodezer::JudgeTypeNode()
{
    S ct = cur.type;
    S nt = next.type;
    string nv = next.value;

    //cout<<cur.value<<"->"<<nv<<endl;

    if(ct==S::Symbol){

        if(nt==S::Sign){
            if(nv=="="){
                //cout<<"  return T::Assignment  "<<endl;
                return T::Assignment;
            }else if(nv=="+"){
                return T::Add;
            }
        }else if(nt==S::Symbol || nt==S::Null){
            //cout<<"  return T::Variable  "<<endl;
            return T::Variable; //变量  
        }

    }else if(ct==S::Int){

        if(nt==S::Sign){

        }else if(nt==S::Symbol || nt==S::Null){
            //cout<<"  return T::Int  "<<endl;
            return T::Int; //int  
        }

    }

    //cout<<"  ############# "<<endl;


    return T::Normal;

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

    // 解析得到语法树
    Node *node = N.Scan();

    //node->childs.at(0);

    cout << "\n\n";

    delete node;



    cout << "\n\n";


    /*/ 打印词法分析结果
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