
#ifndef DEF_NODE_H
#define DEF_NODE_H

#include <string>

using namespace std;
//using namespace def::util;


namespace def {
namespace node {

// 节点类型
enum class Type
{
	Expression,           // 组合表达式

	Assignment,           // 赋值语句

	ArithAdd,             // + 加法计算
	ArithSub,             // - 减法计算
	ArithMul,             // * 乘法计算
	ArithDiv,             // / 除法计算

	FunctionDefine,       // 函数定义
	FunctionCall,         // 函数调用

	// 叶节点
	Variable,           // 变量

	ValueNone,            // 值 none
	ValueBool,            // 值 布尔
	ValueInt,             // 值 整数
	ValueFloat,           // 值 浮点数
	ValueStr,             // 值 字符串

	
}; // --end-- enum class Type


// 节点类
class Node{

	public:

	Type type; //类型
	Node* parent; //父节点指针


}; // --end-- class Node


} // --end-- namespace node
} // --end-- namespace def



#endif
// --end-- DEF_NODE_H

