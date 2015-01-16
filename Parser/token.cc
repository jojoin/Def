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




} // --end-- namespace token
} // --end-- namespace def
