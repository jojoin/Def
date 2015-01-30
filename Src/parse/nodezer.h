
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
			unsigned int sz = words.size();
			if(i+1>=sz) next = nullword;
			if(i>=sz) cur = nullword;
			if(i-1>=sz) prev = nullword;
		}
	};

	void Jump(unsigned int s=1){
		i += s;
        //Read();
	}

	void Clear(){
		i = 0;
		nullword = Word{0,0,Token::State::Null,""};
		prev = cur = next = nullword;
		ctn = TypeNode::Expression;
		//tn_stk.clear();
		//tn_stk.push_back(TypeNode::Expression);
	};


	Node* Scan(Node*); // 扫描构建节点树

	TypeNode JudgeTypeNode(); // 判断当前节点类型
	void Judge(); //执行预判节点类型

	private:

	unsigned int i; // 当前单词位置
	Word prev;  // 上一个单词
	Word cur;   // 当前单词
	Word next;  // 下一个单词

	Word nullword;  // 空单词

	vector<Word>& words; // 单词
	//vector<TypeNode> tn_stk; // 当前节点类型栈
	TypeNode ctn; // 当前节点类型栈

}; // --end-- class Nodezer

} // --end-- namespace node
} // --end-- namespace def



#endif
// --end-- DEF_NODEZER_H

