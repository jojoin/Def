/**
 * 错误抛出
 */

#include <iostream>
#include <cstdlib>
#include <string>
 
#include "error.h"
#include "errordef.h"

using namespace std;
using namespace def::error;


namespace def {
namespace error {


#define ET ErrorType

/**
 * 构造
 */
Error::Error()
{

}



#define E(name, string, prefix) string,
const char* const Error::names[] = {
  ERROR_LIST(E)
};
#undef E



#define E(name, string, prefix) prefix,
const char* const Error::prefixs[] = {
  ERROR_LIST(E)
};
#undef E


/**
 * 抛出错误并退出
 *
 * @param type 错误类型
 * @param code 错误编码
 * @param msg  错误消息
 * @param file 错误文件名
 * @param line 错误行
 * @param posi 错误词
 */
bool Error::Throw(ET type, int code, string msg)
{
	int t = (int)type;
	cerr<<endl<<"- - - - - - - - - - - - - - - -"<<endl;
	cerr<<names[t]<<" error "<<prefixs[t]<<code<<": ";
	cerr<<ErrorDef::Msg(type, code)<<endl; // 错误消息
	cerr<<msg<<endl<<endl;
	exit(1);
}

#undef ET

} // --end-- namespace def
} // --end-- namespace error