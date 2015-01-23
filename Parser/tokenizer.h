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
	unsigned int pos;           // 所属位置
	S type; // 类型
	wstring value;     // 值
};


// 词法分析类
class Tokenizer {

	public:

	Tokenizer(wstring txt);

	// 读取一个字符 并移动指针
	inline wchar_t Read(){
		otok = tok; // 存储
		tok = Peek();
		cursor++; // 移动指针
		return tok;
	};

	// 预先查看后一个字符
	inline wchar_t Peek(){
		ptok = (wchar_t)text[cursor];
		return ptok;
	};

	// 向前一步
	inline void Go(){
		cursor++; // 向后移动指针
	};

	// 返回一步
	inline void Back(){
		cursor--; // 向后移动指针
	};

	// 缓存当前的字符
	inline void Buf(wstring t=L""){
		if(t!=L""){
			buf += t;
		}else{
			buf += tok;
		}
	};

	// 保存当前单词 清空缓存
	inline void Push(S sta=S::Normal){
		if(buf==L""){
			return;
		}
		struct Word wd = {
			line,
			pos,
			sta,
			buf
		};
		words.push_back(wd);
		buf = L"";
	};


	// 清理
	inline void Clear(){
		line = 1; //开始第一行
		pos = 0;
		cursor = 0;
		otok = L' ';
		tok = L' ';
		ptok = L' ';
		buf = L"";
		words.clear();
	};


	// 扫描文本
	vector <Word> & Scan();


	private:

	unsigned int line;  //当前所在行
	unsigned int pos;  //当前所在行
	
	wstring text;  // 需要分析的文本
	wchar_t otok;  // 上一个字符字符
	wchar_t tok;   // 当前字符
	wchar_t ptok;  // 预查看的字符
	unsigned int cursor;  // 当前字符读取位置
	
	wstring buf;   // 缓存的字符
	vector <Word> words;  // 词法分析后的单词列表

}; // --end-- class Token

#undef S

} // --end-- namespace token
} // --end-- namespace def



