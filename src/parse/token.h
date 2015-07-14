#ifndef DEF_VM_TOKEN_H
#define DEF_VM_TOKEN_H


///////
#include <iostream>
///////
#include <string>

#include "../util/str.h"


using namespace std;
using namespace def::util;


namespace def {
namespace parse {

#define TOKEN_LIST(S, K)             \
									\
	/* 符号 */						\
	S(Assing, ":", 0)              \
	S(AssignUp, "::", 0)              \
	S(Comma, ",", 0)              \
	S(Dot, ".", 0)              \
	S(Semicolon, ";", 0)           \
                                \
	S(Add, "+", 0)              \
	S(Sub, "-", 0)              \
	S(Mul, "*", 0)              \
	S(Div, "/", 0)              \
	/*S(Mod, "%", 0)*/              \
    							\
	S(And, "&", 0)              \
	S(Or, "|", 0)               \
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
	S(Equal, "=", 0)              \
	S(More, ">", 0)              \
	S(Less, "<", 0)              \
	S(MoreEqual, ">=", 0)              \
	S(LessEqual, "<=", 0)              \
	S(NotEqual, "~=", 0)              \
	S(Not, "~", 0)               \
									\
	/* 关键字 */					\
	K(Def, "def", 0)                \
	K(Defun, "fn", 0)            \
	K(Class, "class", 0)            \
									\
	K(Return, "ret", 0)          \
	K(Continue, "ctn", 0)      \
	K(Break, "brk", 0)            \
									\
	K(None, "nil", 0)              \
	K(True, "true", 0)              \
	K(False, "false", 0)            \
									\
	K(If, "if", 0)                  \
	K(Elif, "elif", 0)              \
	K(Else, "else", 0)              \
	K(While, "while", 0)            \
	K(For, "for", 0)                \
									\
	K(Try, "try", 0)                \
	K(Catch, "catch", 0)            \
	K(Throw, "throw", 0)            \
									\
	K(Import, "import", 0)          \
									\
	/*K(Print, "print", 0)屏蔽*/
	


class Token {

	public:

	// 状态 包含 word 类型
	enum class State {
		Normal,                 // 默认
		Character,              // 英文字母（变量名和关键字）
		  Variable,                 // 变量名
		  Keyword,                // 关键字
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

		FuncCall,         //函数调用
		ProcCall,         //处理器调用
		ContainerAccess,  //容器访问

		Unknow,       // 不明字符
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
	static bool IsSign(string tok)
	{
		int num = sizeof(signs)/sizeof(signs[0]);
		for(int i=0; i<num; i++)
		{
			if(signs[i] == tok)
			{
				return true;
			}
		}
		return false;
	}

	// 判断字符是否为关键字
	static bool IsKeyword(string tok)
	{
		int num = sizeof(keywords)/sizeof(keywords[0]);
		for(int i=0; i<num; i++)
		{
			if(keywords[i] == tok)
			{
				return true;
			}
		}
		return false;
	}

	// 判断字符是否为合法的变量名
	static bool IsVariable(string str)
	{
		size_t len = str.length();
		for(int i=0; i<len; i++){
			char t = str[i];
			if(i>0&&t>='0'&&t<='9') continue;
			if(t>='a'&&t<='z') continue;
			if(t>='A'&&t<='Z') continue;
			if(t=='_') continue;
			return false;
		}
		return true;

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
	static string GetEscapeChat(string);

	// 判断字符所属状态
	static State GetState(char);
	static State GetState(string);



}; // --end-- class Token


} // --end-- namespace parse
} // --end-- namespace def



#endif
// --end-- DEF_VM_TOKEN_H

