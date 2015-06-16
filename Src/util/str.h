#ifndef DEF_UTIL_STR_H
#define DEF_UTIL_STR_H

/**
 * 工具类
 */



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

	// long 转 string
	static string l2s(const long& num){
		string s="";
		stringstream ss;
		ss << num;//从str输入
		ss >> s;//输出到long
		ss.clear();
		return s;
	}

	// string 转 double
	static double s2d(const string& str){
		double d=0.0;
		stringstream ss;
		ss << str;//从str输入
		ss >> d;//输出到double
		ss.clear();
		return d;
	}

	// 替换所有子字符串
	static void replace_all(string & s, string const & t, string const & w)  
	{  
		string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();  
		while(pos != string::npos){ // found   
			s.replace(pos, t_size, w);   
			pos = s.find(t, pos + r_size );   
		}  
	} 

	

}; // --end-- class Token

} // --end-- namespace util
} // --end-- namespace def


#endif
// --end-- DEF_UTIL_STR_H