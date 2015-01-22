// Copyright 2006-2008 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//#include <stdint.h>

#include "token.h"

namespace def {
namespace token {


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
 * 判断字符所属状态
 */
Token::State Token::GetState(wchar_t tok) {

	if( (tok>=L'a' && tok<=L'z') || 
		(tok>=L'A' && tok<=L'Z') ||
		tok==L'_'
	){ //字母或下划线
		return State::Letter;
	}

	if( tok==L'#' ){ //注释
		return State::Annotation;
	}

	if( tok==L'\n' ){ //换行
		return State::NewLine;
	}

	if( tok>=L'0' && tok<=L'9' ){ //数字
		return State::Number;
	}

	if( IsSign(tok) ){ //有效符号
		return State::Sign;
	}

	if( tok==L'\0' ){ //结束
		return State::End;
	}

	return State::Unknow;

}





} // --end-- namespace token
} // --end-- namespace def
