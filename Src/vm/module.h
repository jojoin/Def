#ifndef DEF_MODULE_H
#define DEF_MODULE_H

/**
 * Def 模块加载
 */

#include <string>
#include <vector>
#include <map>

using namespace std;

#include "../object/object.h"

using namespace def::object;


namespace def {
namespace vm {


// 模块加载处理
class Module {

	public:

	map<string, ObjectModule*> _exist; // 已经加载的模块

	public:

	Module();

	ObjectModule* Load(string name, string basefile=""); // 加载模块
	ObjectModule* LoadSys(string name); // 加载系统模块

	vector<string> MatchFiles(string name, string basefile); // 获取目标文件列表

	// 读取模块文件，并分析执行
	ObjectModule* Create(string path);


};


} // end namespace vm
} // end namespace def



#endif
// --end-- DEF_MODULE_H
