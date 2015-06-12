/**
 * Def 模块加载
 */


#include <fstream>

#include "module.h"

#include "../util/path.h"

using namespace std;

using namespace def::object;
using namespace def::util;

namespace def {
namespace vm {


// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);


Module::Module()
{
	_exist = map<string, ObjectModule*>();
}



/**
 * 加载模块
 * @param  name  模块名称
 * @param  basefile  当前文件
 */
ObjectModule* Module::Load(string name, string basefile)
{
	ObjectModule* sysmd = LoadSys(name);
	if(sysmd){
		return sysmd; // 返回系统模块
	}
	vector<string> pts = MatchFiles(name, basefile);
	size_t i = 0, len = pts.size();
	while(i<len){
		string file = pts[i];
		// cout<<file<<endl;
		map<string, ObjectModule*>::iterator iter = _exist.find(file);
		// 是否已经缓存
		if(iter!=_exist.end())
		{
		    return iter->second; // 返回缓存
		}
		// 读取文件
		ifstream fin(file, ios::in);
		if(fin)
		{	// 存在文件
			fin.close();
			ObjectModule* mod = Create(file); // 建立模块
			_exist.insert(map<string, ObjectModule*>::value_type(file, mod));
			// _exist[file] = mod; // 缓存
			return mod;
		}
		i++;
	}

	return NULL; // 未查找到模块
}



/**
 * 读取模块文件，并分析执行
 */
ObjectModule* Module::Create(string file)
{
	return NULL;
}




/**
 * 加载系统模块
 */
ObjectModule* Module::LoadSys(string name)
{

	return NULL; // 未查找到模块
}




/**
 * 获取目标文件列表
 */
vector<string> Module::MatchFiles(string name, string basefile)
{
	vector<string> paths;
	string basepath = basefile=="" ? Path::cwd() : Path::getDir(basefile);
	string tar = Path::join(basepath, name); //合并
	string ext = Path::getFileExt(name);
	// cout<<"ext="<<ext<<endl;
	// cout<<"tar="<<tar<<endl;

	if(ext!=""){
		paths.push_back( tar );
	}else{
		string d = Path::div();
		// paths.push_back( tar );
		paths.push_back( tar+".d" );
		paths.push_back( tar+d+"index.d" );
		string fname = Path::getFileName(name);
		paths.push_back( tar+d+fname+".d" );
	}

	return paths;
}





} // end namespace vm
} // end namespace def


