/**
 * 语法树节点
 */

#include "node.h"

namespace def {
namespace parse {

Node::~Node(){}; // 纯虚析构函数的定义


#define NT NodeType


/**
 * 获取变量类型名称
 */
string Node::GetTypeName(Node *n)
{
	if(n==NULL){
		return "";
#define N(name, priority)
#define D(name, priority) }else if(n->type==NT::name){ return #name;
	NODELIST(N, D)
#undef N
#undef D
	}

}



#undef NT   // NodeType


} // end namespace parse
} // end namespace def
