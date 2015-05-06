#ifndef DEF_TOKENIZER_H
#define DEF_TOKENIZER_H
/**
 * 词法分析器 
 * 
 * 
 */


#include <string>
#include <vector>

#include "token.h"
#include "../util/log.h"
#include "../util/str.h"
#include "../module/error.h"


using namespace std;
using namespace def::token;
using namespace def::error;


namespace def {
namespace token {

#define S Token::State

// 单词
struct Word {
	size_t line;          // 代码行
	size_t posi;          // 所属位置
	S type; // 类型
	string value;     // 值
};


// 词法分析类
class Tokenizer {

	public:

	Tokenizer(bool, string, vector<Word>*);

	// 抛出错误
	inline bool Error(int code){
		string msg = filepath + " ("
			+Str::l2s(line) + ","
			+Str::l2s(word_pos)
			+") : " + tok;
		//tok;
		return Error::Throw(ErrorType::Token, code, msg);
	};

	// 读取一个字符 并移动指针
	inline string Read(){
		pprev_tok = prev_tok; // 存储
		prev_tok = tok; // 存储
		tok = Peek();
		Jump();
		return tok;
	};

	// 预先查看后一个字符
	/*
	inline char Peek(size_t c=1){
		return (char)text[cursor+(--c)];
	};
	*/
	inline string Peek(size_t c=1){
		size_t idx = cursor+(--c);
    	// cout << "Peek " << idx << endl;
		try{
			return text.substr(idx,1);
		}catch(const exception& e){
			return "\0";
		}
	};

	// 向前一步
	inline void Jump(size_t c=1){
		cursor += c; // 向后移动指针
	};

	// 返回一步
	inline void Back(size_t c=1){
		cursor -= c; // 向后移动指针
	};

	// 缓存当前的字符
	inline void Buf(string t=""){
		if(t!=""){
			buf += t;
		}else{
			buf += tok;
		}
	};

	// 弹出一个缓存的字符
	inline void Pop(size_t n=1){
		buf.erase(buf.size()-n, n);
	};

	// 保存当前单词 清空缓存
	inline void Push(S);

	// 清理
	inline void Clear(){
		line = 1;
		line_start = 0;
		word_pos = 1;
		cursor = 0;
		pprev_tok = "";
		prev_tok = "";
		tok = "";
		buf = "";
		words->clear();
	};


	// 扫描文本
	void Scan();


	private:

	size_t line;  //当前所在行号
	size_t line_start;  //记录开始行号，用于跨行命令
	size_t word_pos;  //上次预读字符位置

	string filepath;  // 需要分析的文件名
	string text;  // 需要分析的文本
	// char pprev_tok; // 上上一个字符
	// char prev_tok; // 上一个字符
	// char tok;      // 当前字符
	string pprev_tok; // 上上一个字符
	string prev_tok; // 上一个字符
	string tok;      // 当前字符
	size_t cursor;  // 当前字符读取位置
	
	string buf;   // 缓存的字符
	vector<Word>* words;  // 词法分析后的单词列表

}; // --end-- class Token

#undef S

} // --end-- namespace token
} // --end-- namespace def


#endif
// --end-- DEF_TOKENIZER_H



