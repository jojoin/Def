/** 
 * Def 对象基础
 */

#ifndef DEF_OBJECT_H
#define DEF_OBJECT_H

#include <string>

using namespace std;


namespace def {
namespace object {


// Def类型对象
struct Type{
	string tp_name; //类型名称

};
	

// Def基础对象
struct Def{
	unsigned int ob_refcnt; //引用计数
	Type *ob_type; //类型对象
};
	
} // --end-- namespace object
} // --end-- namespace def



#endif
// --end-- DEF_OBJECT_H








