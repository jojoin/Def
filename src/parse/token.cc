// Copyright 2006-2008 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//#include <stdint.h>

#include "token.h"

namespace def {
namespace parse {


	// 符号数组
#define S(name, string, precedence) string,
#define K(name, string, precedence)
	const char* const Token::signs[] = {
	  TOKEN_LIST(S, K)
	};
#undef S
#undef K

	// 关键字数组
#define S(name, string, precedence) 
#define K(name, string, precedence) string,
	const char* const Token::keywords[] = {
	  TOKEN_LIST(S, K)
	};
#undef S
#undef K


/**
 * 获取转义字符
 */
string Token::GetEscapeChat(string tok)
{
	char tk = (char)tok[0];
	char re = GetEscapeChat(tk);
	return {re};
}
char Token::GetEscapeChat(char tok)
{
	switch (tok) {
		case '\'':  // fall through
		case '"' :  // fall through
		case '\\': 
		return tok;
		case '0' : return '\0'; //NULL空字符
		case 'a' : return '\a'; //响铃
		case 'b' : return '\b'; //退格
		case 'f' : return '\f'; //换页
		case 'n' : return '\n'; //换行
		case 'r' : return '\r'; //回车
		case 't' : return '\t'; //水平制表
	}
	return tok;
}

/**
 * 判断字符所属状态
 */
Token::State Token::GetState(string tok)
{
	return GetState(tok[0]);
}
Token::State Token::GetState(char tok)
{
	if( (tok>='a' && tok<='z') || 
		(tok>='A' && tok<='Z') ||
		tok=='_'
	){ //字母或下划线
		return State::Character;
	}

	if( tok==' ' || tok=='	'){ //空格或tab
		return State::Space;
	}

	if( tok>='0' && tok<=L'9' ){ //数字
		return State::Number;
	}

	if( tok=='#' ){ //注释
		return State::Annotation;
	}

	if( tok=='"' ){ //字符串
		return State::DQuotation;
	}else if( tok=='\'' ){
		return State::Quotation;
	}

	if( tok=='\n' ){ //换行
		return State::NewLine;
	}

	if( IsSign(tok) ){ //有效符号
		return State::Sign;
	}

	if( tok=='\0' ){ //结束
		return State::End;
	}

	return State::Unknow;
}





} // --end-- namespace parse
} // --end-- namespace def
