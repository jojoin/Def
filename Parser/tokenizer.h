/**
 * 词法分析器 
 * 
 * 
 */


#include <string>
#include <vector>

#include "token.h"


using namespace std;


namespace def {
namespace token {


class Tokenizer {

	public:

	Tokenizer(wstring txt);

	// 读取一个字符 并移动指针
	inline wchar_t Read(){
		otok = tok; // 存储
		tok = Peek();
		pos++; // 移动指针
		return tok;
	};

	// 预先查看后一个字符
	inline wchar_t Peek(){
		ptok = (wchar_t)text[pos];
		return ptok;
	};

	// 向前一步
	inline void Go(){
		pos++; // 向后移动指针
	};

	// 返回一步
	inline void Back(){
		pos--; // 向后移动指针
	};

	// 缓存当前的字符
	inline void Buf(){
		buf += tok;
	};

	// 去除缓存的字符 作为一个单词保存 清空缓存
	inline void PopBuf(){
		if(buf==L""){
			return;
		}
		words.push_back(buf);
		buf = L"";
	};


	// 清理
	inline void Clear(){
		otok = L' ';
		tok = L' ';
		ptok = L' ';
		pos = 0;
		buf = L"";
		words.clear();
	};


	// 扫描文本
	vector <wstring> & Scan();







	private:

	// 需要分析的文本
	wstring text;
	// 上一个字符字符
	wchar_t otok;
	// 当前字符
	wchar_t tok;
	// 预查看的字符
	wchar_t ptok;
	// 当前字符读取位置
	unsigned int pos;

	// 缓存的字符
	wstring buf;
	// 词法分析后的单词列表
	vector <wstring> words;

}; // --end-- class Token


} // --end-- namespace token
} // --end-- namespace def



