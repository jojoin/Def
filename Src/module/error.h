#ifndef DEF_ERROR_H
#define DEF_ERROR_H

/**
 * 语言错误抛出模块
 * 当发生词法或语法错误时，抛出错误
 * 当发生运行时错误时，抛出错误
 *
 *
 */


#include <string>

#include "../parse/tokenizer.h"


using namespace std;
using namespace def::token;


namespace def {
namespace error {

#define ERROR_LIST(E)                                 \
	E(System, "System", "S")  /* 系统错误 */          \
	E(Find,   "Find",   "F")  /* 文件或加载错误 */    \
	E(Token , "Token",  "T")  /* 词法错误 */          \
	E(Node,   "Node",   "N")  /* 语法错误 */          \
	E(Run,    "Run",    "R")  /* 运行时错误 */



// 错误类型
#define E(name, string, prefix) name,
enum class ErrorType {
	ERROR_LIST(E)
	_Num // 数量
}; // --end-- enum class ErrorType
#undef E



#define ET ErrorType

// 类
class Error{

	private:

	static const char* const names[(int)ET::_Num];
	static const char* const prefixs[(int)ET::_Num];
	
	public:

	Error();

	static bool Throw(ET, int, string="", string="", size_t=0, size_t=0); // 抛出错误并退出
	// 抛出特定类型的错误
	static bool System(int, string="");


}; // --end-- class Error

#undef ET // ErrorType

} // --end-- namespace error
} // --end-- namespace def




#endif
// --end-- DEF_ERROR_H
