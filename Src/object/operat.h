/** 
 * Def 对象 操作
 */

#ifndef DEF_OPERAT_H
#define DEF_OPERAT_H

#include <string>

#include "object.h"

using namespace std;
using namespace def::object;


namespace def {
namespace operat {

// 类型转换
class Conversion
{
	public:
	static bool Bool(DefObject*); // 转换为 bool 类型
};


// 数学计算
class Math
{
	public:
	static bool Add(); // 转换为 bool 类型
};


} // --end-- namespace operat
} // --end-- namespace def



#endif
// --end-- DEF_OPERAT_H

