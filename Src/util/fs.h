#ifndef DEF_UTIL_FS_H
#define DEF_UTIL_FS_H

/**
 * 文件读取
 */

#include <iostream>
#include <fstream> 
#include <cstdlib>
#include <string>


using namespace std;

namespace def {
namespace util {

class Fs {

	public:

	// 判断文件是否存在
	static bool Exist(string &fp)
	{
		ifstream fin(fp, ios::in);
		return fin ? true : false;
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


}; // --end-- class Fs

} // --end-- namespace util
} // --end-- namespace def


#endif
// --end-- DEF_UTIL_SYS_H