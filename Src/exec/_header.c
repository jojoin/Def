/**
 * Def 执行上下文
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <map>

#include "../global.h"

#include "../vm/exec.h"
#include "../util/str.h"
#include "../util/fs.h"
#include "../util/path.h"

using namespace std;

using namespace def::parse;
using namespace def::object;
using namespace def::util;


namespace def {
namespace vm {

// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);


#define DO DefObject
#define OT ObjectType
#define NT NodeType

// 将环境变量初始化
#define LOCALIZE_module    Module *_module = _envir._module;
#define LOCALIZE_gc        Gc     *_gc     = _envir._gc;
#define LOCALIZE_stack     Stack  *_stack  = _envir._stack;
#define LOCALIZE_node      Node   *_node   = _envir._node;
