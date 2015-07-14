#ifndef DEF_VM_ABNOR_H
#define DEF_VM_ABNOR_H

/**
 * Def 执行中断抛出
 */


#include <string>

#include "../object/object.h"

using namespace std;

using namespace def::object;

namespace def {
namespace vm {


// 抛出类型
enum class AbnorType{
    Return,    // 函数执行返回
    Continue,  // 循环继续
    Break,     // 循环退出
    
    Throw,     // 自定义抛出
};


// 抛出（异常）
class Abnor
{
	public:

    AbnorType type; // 类型
    string msg;     // 消息
    DefObject *obj; // 抛出对象

	public:

    // 构造函数,参数为错误代码
    Abnor(AbnorType t, string m="", DefObject*o=NULL)
    : type(t)
    , msg(m)
    , obj(o)
    {}
    inline AbnorType GetType(){ return type; }
    inline string GetMsg(){ return msg; }
    inline DefObject* GetObject(){ return obj; }
};




} // --end-- namespace vm
} // --end-- namespace def

#endif
// --end-- DEF_VM_ABNOR_H


