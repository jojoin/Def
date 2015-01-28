/**
 * 词法分析器 
 * 
 * 
 */


#include <string>
#include <vector>

#include "token.h"


using namespace std;
using namespace def::token;


namespace def {
namespace token {

#define S Token::State

// 单词
struct Word {
	unsigned int line;          // 代码行
	unsigned int posi;          // 所属位置
	S type; // 类型
	string value;     // 值
};


// 词法分析类
class Tokenizer {

	public:

	Tokenizer(bool, string, vector<Word>&);

	// 读取一个字符 并移动指针
	inline char Read(){
		pprev_tok = prev_tok; // 存储
		prev_tok = tok; // 存储
		tok = Peek();
		Jump();
		return tok;
	};

	// 预先查看后一个字符
	inline char Peek(unsigned int c=1){
		return (char)text[cursor+(--c)];
	};

	// 向前一步
	inline void Jump(unsigned int c=1){
		cursor += c; // 向后移动指针
	};

	// 返回一步
	inline void Back(unsigned int c=1){
		cursor -= c; // 向后移动指针
	};

	// 缓存当前的字符
	inline void Buf(char t='\0'){
		if(t!='\0'){
			buf += t;
		}else{
			buf += tok;
		}
	};

	// 弹出一个缓存的字符
	inline void Pop(unsigned int n=1){
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
		pprev_tok = '\0';
		prev_tok = '\0';
		tok = '\0';
		buf = "";
		words.clear();
	};


	// 扫描文本
	void Scan();


	private:

	unsigned int line;  //当前所在行号
	unsigned int line_start;  //记录开始行号，用于跨行命令
	unsigned int word_pos;  //上次预读字符位置

	string text;  // 需要分析的文本
	char pprev_tok; // 上上一个字符
	char prev_tok; // 上一个字符
	char tok;      // 当前字符
	unsigned int cursor;  // 当前字符读取位置
	
	string buf;   // 缓存的字符
	vector<Word> & words;  // 词法分析后的单词列表

}; // --end-- class Token

#undef S

} // --end-- namespace token
} // --end-- namespace def



