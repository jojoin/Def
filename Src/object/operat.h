/** 
 * Def 对象 操作
 */

#ifndef DEF_OPERAT_H
#define DEF_OPERAT_H

#include <string>

#include "object.h"

using namespace std;


namespace def {
namespace object {



// 类型转换
class Conversion
{
	public:
	static bool Bool(DefObject*); // 转换为 bool 类型
	static string String(DefObject*); // 转换为 string 类型
	static long Long(DefObject*); // 转换为 long 类型

	static bool CanLong(DefObject*); // 是否可以转换为 long
};




// 数学计算
class Math
{
	public:
	static bool Add(); // 转换为 bool 类型
};


} // --end-- namespace object
} // --end-- namespace def



#endif
// --end-- DEF_OPERAT_H

