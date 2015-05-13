#ifndef DEF_NODEZER_H
#define DEF_NODEZER_H

#include <string>
#include <vector>
#include <stack>  
#include <exception>

#include "tokenizer.h"
#include "node.h"
#include "../module/error.h"

using namespace std;
using namespace def::token;
using namespace def::node;
using namespace def::error;

// NodeType
#define T NodeType

namespace def {
namespace node {


class Nodezer {

	public:

	Nodezer(vector<Word>*, string);

	// 错误中断抛出
	inline bool Error(int code){
		return Error(code, cur);
	};

	// 错误中断抛出
	inline bool Error(int code, Word wd){
		string msg = filepath + " ("
			+Str::l2s(wd.line) + ","
			+Str::l2s(wd.posi)
			+") : " + wd.value;
		//tok;
		return Error::Throw(ErrorType::Node, code, msg);
	};

	inline void Read(){
		try{
			//if(i>0) prev = words->at(i-1);
			cur = words->at(i);
			//next = words->at(i+1);
			// cout<<"Read(): i="<<i<<","<<cur.value<<endl;
		}catch(const exception& e){
			//size_t sz = words->size();
			//if(i+1>=sz) next = endword;
			//if(i>=sz) cur = endword;
			//if(i-1>=sz) prev = endword;
			cur = endword;
		}

	};

	inline void Move(int s=1){
		i += s;
        Read();
	}

	inline void Clear(){
		i = 0;
		cur = endword = Word{0,0,Token::State::End,""};
		nodebuf = NULL;

		//prev = cur = next = endword;
		//ctn = NodeType::Expression;
		//tn_stk.clear();
		//tn_stk.push_back(NodeType::Expression);
	};

	static bool IsType(T,T,T,T,T,T,T,T,T,T,T); // 节点类型是否匹配
	T GetNodeType(Word&); // 获得节点类型
	T CurNodeType(); // 判断当前节点类型
	//Node* CreatNode(int, Node*, Node*); //从当前单词新建节点
	Node* CreatNode(); //从当前单词新建节点
	//Node* CreatMolecule(Node*,Node*); // 构建表达式分子，例如括号优先级，数组结构等
	Node* ParseNode(Node*); //解析当前节点
	Node* AssembleNode(Node*,Node*); //组合当前两个节点
	Node* Express(Node*); // 扫描单词 构建表达式
	Node* Group(T type=T::Group); // 构建表达式组
	Node* BuildAST();   // 扫描单词 构建语法树
	int GetPriority(Node*); // 取得节点优先级

	bool IsGroupEnd(); // 表达式组结束标志

	private:

	size_t i; // 当前单词位置
	//Word prev;  // 上一个单词
	Word cur;   // 当前单词
	//Word next;  // 下一个单词

	Word endword;  // 空单词

	T cnt; // 当前节点类型
	//stack<Node*> nodebuf; // 缓存待用的节点
	Node* nodebuf; // 缓存待用的节点

	vector<Word>* words; // 单词列表
	string filepath; //编译文件

}; // --end-- class Nodezer

} // --end-- namespace node
} // --end-- namespace def

# undef T // NodeType

#endif
// --end-- DEF_NODEZER_H

