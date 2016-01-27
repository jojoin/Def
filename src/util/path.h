#ifndef DEF_UTIL_PATH_H
#define DEF_UTIL_PATH_H

/**
 * 工具类
 */

#include <iostream>
#include <cstdlib>
#include <string>

#include "./str.h"



#ifdef WINDOWS
#include <direct.h>
#else
#include <unistd.h>
#endif


using namespace std;

namespace def {
namespace util {

class Path {

	public:
        
	static char D;  // 路径分割斜杠
	static string m_cwd; // 当前路径
    
	// 获取路径分割字符
	static string div()
	{
		return D=='/' ? "/" : "\\";
	}

	// 兼容平台处理
	static string fix(string path)
	{
        if (D=='/') {
            Str::replace_all(path, "\\", "/");
        }
        else {
            Str::replace_all(path, "/", "\\");
        }
        return path;
	}
    
	// 获取当前目录
	static string cwd()
	{
        if (m_cwd=="") {
		    char buffer[1024];
	        _getcwd(buffer, 1024);
	        // cout<<buffer<<endl;
            m_cwd = (string)buffer;
        }
	    return m_cwd;
	}

	// 获取绝对路径
	static string absolute(const string & path)
	{
        return join(cwd(), path);
	}

	// 获取文件名称
	static string getFileName(const string & file)
	{
		int pos = file.find_last_of(D);
		string s( file.substr(pos+1) );
		return s;
	}


	// 获取文件扩展名
	static string getFileExt(const string & file)
	{
		string name = getFileName(file);
		int pos = name.find_last_of('.');
		if(pos==-1){
			return "";
		}
		return name.substr(pos+1);
	}

	// 获取文件路径
	static string getDir(const string & file)
	{
		int pos = file.find_last_of(D);
		return file.substr(0, pos);
	}

	// 获取文件名称（不带后缀）
	static string getName(string file)
	{
		int len = file.length();
		if( D==file[len-1] ){
			file = file.substr(0, len-1);
		}

		int pos = file.find_last_of(D);
		int px = file.find_last_of('.');
		if(px==-1){
			return file.substr(pos+1);
		}else{
			return file.substr(pos+1, px-pos-1);
		}
	}


	// 路径合并
	static string join(string p1, string p2)
    {
        p1 = fix(p1);
        p2 = fix(p2);
        if (""!=getFileExt(p1)) {
            p1 = getDir(p1); // 文件名路径
        }

		if(p2==""){
			return p1;
		}

		int p1l = p1.length();
		int p2l = p2.length();

		if( (p1l>0&&p2[0]==D) || (p2l>1&&p2[1]==':') ){
			return p2; // 绝对路径，直接返回
		}

		if( D==p1[p1l-1] ){
			p1 = p1.substr(0, p1l-1);
		}

		// cout<<"p1 = "<<p1<<endl;
		// cout<<"p2 = "<<p2<<endl;

		string up = "..."; up[2] = D;
		string cr = ".."; cr[1] = D;

		while(p2.substr(0, 3)==up){ // 父级目录 ../
			// cout<<"p2 .."<<endl;
			p1 = getDir(p1);
			p2 = p2.substr(3);
		}

		while(p2.substr(0, 2)==cr){ // 忽略 ./
			// cout<<"p2 ."<<endl;
			p2 = p2.substr(2);
		}

		// cout<<"p1 = "<<p1<<endl;
		// cout<<"p2 = "<<p2<<endl;
		
		return p1 + div() + p2; // 返回组合

	}

}; // --end-- class Sys





} // --end-- namespace util
} // --end-- namespace def


#endif