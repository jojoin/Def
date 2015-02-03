
#ifndef DEF_TOKEN_H
#define DEF_TOKEN_H


///////
#include <iostream>
///////
#include <string>

#include "../Util/str.h"


using namespace std;
using namespace def::util;


namespace def {
namespace token {

#define TOKEN_LIST(S, K)             \
									\
	/* 符号 */						\
	S(Assing, "=", 0)              \
	S(Comma, ",", 0)              \
	S(Dot, ".", 0)              \
	S(Colon, ":", 0)              \
	S(Semicolon, ";", 0)              \
	/*S(Conditional, "?", 0)*/              \
	/*S(Exclamation, "!", 0)*/              \
	/*S(Tilde, "~", 0)*/              \
                                \
	S(Add, "+", 0)              \
	S(Sub, "-", 0)              \
	S(Mul, "*", 0)              \
	S(Div, "/", 0)              \
	/*S(Mod, "%", 0)*/              \
    							\
	S(Lparen, "(", 0)              \
	S(Rparen, ")", 0)              \
	S(Lbrack, "[", 0)              \
	S(Rbrack, "]", 0)              \
	S(Lbrace, "{", 0)              \
	S(Rbrace, "}", 0)              \
									\
	S(DQuotation, "\"", 0)              \
	S(Quotation, "\'", 0)              \
									\
	S(Pound, "#", 0)              \
									\
	S(Equal, "==", 0)              \
	/*S(Array, ":[", 0)*/              \
									\
	/* 关键字 */					\
	K(If, "if", 0)              \
	K(While, "while", 0)              \
	K(And, "and", 0)              \
	K(Or, "or", 0)              \
	K(Is, "is", 0)              \
	K(As, "as", 0)


class Token {

	public:

	// 状态 包含 word 类型
	enum class State {
		Normal,                 // 默认
		Identifier,             // 标识符 包含关键字
		  Symbol,                 // 变量名
		  Keyword,                // 关键字
		Character,              // 英文字母
		Sign,                   // 符号
		Number,                 // 数字
		  Int,                    // 整形
		  Float,                  // 浮点型
		String,                 // 字符串
		  DQuotation,    //双引号
		  Quotation,     //单引号
		  BlockDQuotation,//块字符串
		  BlockQuotation, //块字符串
		Annotation,   // 单行注释
		BlockAnnotation,   // 块注释
		Space,        // 空格、tab等制表符
		NewLine,      // 换行
		
		Unknow,       // 不明字符
		Null,         // 不存在
		End           // 结束符
	};


	// 符号与关键字具体值
#define S(name, string, precedence) name,
	enum class Value {
		TOKEN_LIST(S, S)
		_Num // 符号数量
	};
#undef S



	// 符号与关键字具体值
#define S(name, string, precedence) name,
#define K(name, string, precedence)
	enum class Sign {
		TOKEN_LIST(S, K)
		_Num // 符号数量
	};
#undef S
#undef K

	// 符号与关键字具体值
#define S(name, string, precedence)
#define K(name, string, precedence) name,
	enum class Keyword {
		TOKEN_LIST(S, K)
		_Num // 关键字数量
	};
#undef S
#undef K

	//private:

	static const char* const signs[(int)Sign::_Num];
	static const char* const keywords[(int)Keyword::_Num];


/////////////////////  方法   //////////////////////

	// 判断字符是否为符号
	static bool IsSign(char tok)
	{
		string str = " ";
		str[0] = tok;
		return IsSign(str);
	}
	static bool IsSign(string str)
	{
		int num = (int)Sign::_Num;
		for(int i=0; i<num; i++)
		{
			if(signs[i] == str)
			{
				return true;
			}
		}
		return false;
	}

	// 判断字符是否为关键字
	/*
	static bool IsKeyword(Value tok) {
		return value_type[tok] == 'K'; // tok is unsigned
	}*/
	static bool IsKeyword(string tok)
	{
		int num = (int)Sign::_Num;
		for(int i=0; i<num; i++)
		{
			if(keywords[i] == tok)
			{
				return true;
			}
		}
		return false;
	}

	//判断是否为浮点数
	static bool IsFloat(string str)
	{
		// npos 表示未找到
		if(str.find(".")!=string::npos){
			return true;
		}
		return false;
	}

	// 获取转义字符
	static char GetEscapeChat(char);

	// 判断字符所属状态
	static State GetState(char);



}; // --end-- class Token

} // --end-- namespace token
} // --end-- namespace def



#endif
// --end-- DEF_TOKEN_H

