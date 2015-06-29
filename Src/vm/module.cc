/**
 * Def 模块加载
 */


#include <fstream>

#include "module.h"

#include "../util/path.h"
#include "../util/fs.h"

using namespace std;

using namespace def::object;
using namespace def::util;

namespace def {
namespace vm {


// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);


Module::Module()
{
}



/**
 * 加载模块
 * @param  name  模块名称
 * @param  basefile  当前文件
 */
ObjectModule* Module::GetCache(string file)
{
	map<string, ObjectModule*>::iterator iter = _exist.find(file);
	// 是否已经缓存
	if(iter!=_exist.end())
	{
	    return iter->second; // 返回缓存
	}

	return NULL; // 未查找到系统模块或缓存
}



/**
 * 缓存模块
 */
void Module::SetCache(string file, ObjectModule* cache)
{
    _exist.insert(map<string, ObjectModule*>::value_type(file, cache));
}

/**
 * 清除模块缓存
 */
void Module::ClearCache(string name, string basefile)
{
}


// 检测是否为系统模块名称
bool Module::IsSysmodName(string n)
{
	if(
		n=="sys" ||
		n=="fs"
	){
		return true;
	}

	return false;
}

/**
 * 获取目标文件列表
 */
string Module::MatchFile(string name, string basefile)
{
	vector<string> paths;
	string basepath = basefile=="" ? Path::cwd() : Path::getDir(basefile);
	string tar = Path::join(basepath, name); //合并
	string ext = Path::getFileExt(name);
	// cout<<"ext="<<ext<<endl;
	// cout<<"tar="<<tar<<endl;
	if(ext!=""){
		return Fs::Exist(tar) ? tar : "";
	}else{
		string d = Path::div();

		string t1 = tar + ".d";
		if( Fs::Exist(t1) ) return t1;

		string t2 = tar + d + "index.d";
		if( Fs::Exist(t2) ) return t2;

		string t3 = tar + d + Path::getFileName(name) + ".d";
		if( Fs::Exist(t3) ) return t3;
	}

	return ""; // 未能匹配
}






} // end namespace vm
} // end namespace def


