
/**
 * 工具类
 */

#ifndef DEF_UTIL_STR_H
#define DEF_UTIL_STR_H


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

	

}; // --end-- class Token

} // --end-- namespace util
} // --end-- namespace def


#endif
// --end-- DEF_TOKEN_H