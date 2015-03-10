#ifndef DEF_NODEZER_H
#define DEF_NODEZER_H

#include <string>
#include <vector>
#include <exception>

#include "tokenizer.h"
#include "node.h"

using namespace std;
using namespace def::token;
using namespace def::node;

// NodeType
#define T NodeType

namespace def {
namespace node {


class Nodezer {

	public:

	Nodezer(vector<Word>&);

	void Read(){
		//cout<<"i: "<<i<<endl;
		try{
			if(i>0) prev = words.at(i-1);
			cur = words.at(i);
			next = words.at(i+1);
		}catch(const exception& e){
			size_t sz = words.size();
			if(i+1>=sz) next = endword;
			if(i>=sz) cur = endword;
			if(i-1>=sz) prev = endword;
		}

	};

	inline void Move(int s=1){
		i += s;
        //Read();
	}

	inline void Clear(){
		i = 0;
		endword = Word{0,0,Token::State::End,""};
		prev = cur = next = endword;
		//ctn = NodeType::Expression;
		//tn_stk.clear();
		//tn_stk.push_back(NodeType::Expression);
	};

	static bool IsType(T,T,T,T,T,T,T,T,T,T,T); // 节点类型是否匹配
	T GetNodeType(Word&); // 获得节点类型
	T CurNodeType(); // 判断当前节点类型
	Node* CreatNode(int, Node*, Node*); //从当前单词新建节点
	Node* Express(Node*,T); // 扫描单词 构建表达式
	Node* Group(); // 构建表达式组
	Node* BuildAST();   // 扫描单词 构建语法树
	//Node* Tuple(); // 构建元组
	//Node* Array(); // 构建数组
	//Node* IfElse(); // 构建数组

	private:

	size_t i; // 当前单词位置
	Word prev;  // 上一个单词
	Word cur;   // 当前单词
	Word next;  // 下一个单词

	Word endword;  // 空单词

	T cnt; //当前节点类型

	vector<Word>& words; // 单词列表

}; // --end-- class Nodezer

} // --end-- namespace node
} // --end-- namespace def

# undef T // NodeType

#endif
// --end-- DEF_NODEZER_H

