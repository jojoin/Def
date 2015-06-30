#ifndef DEF_UTIL_FS_H
#define DEF_UTIL_FS_H

/**
 * 文件读取
 */

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream> 
#include <cstdlib>
#include <string>
#include <vector>

#include <iomanip> 
#include <io.h>
#include <string.h>
#include <ctime>

#include "../global.h"

#ifdef WINDOWS
#include <direct.h>
#else
#include <unistd.h>
#endif


using namespace std;

namespace def {
namespace util {

class Fs {

	public:

	// 判断文件是否存在
	static bool Exist(string &file)
	{
		ifstream fin(file, ios::in);
		return fin ? true : false;
	}

	// 判断目录是否存在
	static bool ExistDir(const string &path)
	{
		return _access(path.c_str(), 0)==0 ? true : false;
	}

	// 读取文件，得到文本string
	static string ReadFile(string &fp)
	{
		// 一次性读取文件所有内容至string
		// cout << "isFile" << endl;
		ifstream in(fp, ios::in);
		// cout << "ifstream" << endl;
		istreambuf_iterator<char> beg(in), end;
		// cout << "istreambuf_iterator" << endl;
		string strdata(beg, end);
		// cout << "strdata" << endl;
		in.close();
		// cout << "close" << endl;
		return strdata;
	}

	// 读取文件，得到文本string
	static bool WriteFile(string &fp, string &str)
	{
		//打开文件用于写，若文件不存在就创建它
		ofstream f1(fp);
		//打开文件失败则结束运行
		if(!f1)	return false;
		//使用插入运算符写文件内容
		f1 << str;
		f1.close();
		return true;
	}

	// 获取目录下的所有文件或者目录
	// type==0 仅获取文件名 
	// type==1 仅获取目录名
	// type==2 获取文件和目录
	static void GetChilds(string path, vector<string>& files, int type=0)
	{
	    //文件句柄
	    // HANDLE   hFile   =   0;    
	    long     hFile   =   0;    
	    //文件信息    
	    struct _finddata_t fileinfo;    
	    string p;  
	    if((hFile = _findfirst(p.assign(path).append("/*").c_str(),&fileinfo)) != -1){
	        do{    
	            //如果是目录, 迭代之
	            if((fileinfo.attrib & _A_SUBDIR)) {    
	                if(strcmp(fileinfo.name,".") != 0  &&
	                   strcmp(fileinfo.name,"..") != 0)
	                { 
	                    //getFiles(   p.assign(path).append("/").append(fileinfo.name), files   ); // 递归
	                    // 加入目录名
	                    if(type>0) files.push_back(   fileinfo.name  );
	                }   
	            }else{    
	                // files.push_back(   p.assign(path).append("/").append(fileinfo.name)  );
	                // 加入文件名
	                if(type!=1) files.push_back(   fileinfo.name  );
	            }    
	        }while(_findnext(hFile, &fileinfo) == 0);    
	  
	        _findclose(hFile);    
	    }  
	}  



}; // --end-- class Fs

} // --end-- namespace util
} // --end-- namespace def


#endif
// --end-- DEF_UTIL_SYS_H