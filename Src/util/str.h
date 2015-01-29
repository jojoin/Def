
/**
 * 工具类
 */

#ifndef DEF_UTIL_STR_H
#define DEF_UTIL_STR_H


#include <sstream>
#include <string>

using namespace std;


namespace def {
namespace util {


class Str {

	public:

	// wstring 转 string
	static string ws2s(const wstring& wstr){
		std::string str(wstr.length(), ' ');
        std::copy(wstr.begin(), wstr.end(), str.begin());
        return str;
	}

	// string 转 wstring
	static wstring ws2s(const string& str){
		std::wstring wstr(str.length(),L' ');
        std::copy(str.begin(), str.end(), wstr.begin());
        return wstr; 
	}

	// string 转 long
	static long s2l(const string& str){
		long l=0;
		stringstream ss;
		ss << str;//从str输入
		ss >> l;//输出到long
		ss.clear();
		return l;
	}

	

}; // --end-- class Token

} // --end-- namespace util
} // --end-- namespace def


#endif
// --end-- DEF_UTIL_STR_H