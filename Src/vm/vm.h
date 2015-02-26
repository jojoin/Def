#ifndef DEF_VM_H
#define DEF_VM_H

/**
 * Def 虚拟机（解释器）
 */


#include <string>

#include "../parse/nodezer.h"


using namespace std;


namespace def {
namespace vm {


class Vm {

	public:

	bool Run(string); // 运行 Def 脚本

};


} // --end-- namespace vm
} // --end-- namespace def

#endif
// --end-- DEF_VM_H