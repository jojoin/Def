#ifndef DEF_UTIL_STR_H
#define DEF_UTIL_STR_H

/**
 * 工具类
 */

#include <sstream>
#include <string>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype> 


namespace def {
namespace util {

using namespace std;


class Str {

    public:

    // wstring 转 string
    static string ws2s(const wstring& wstr){
        std::string str(wstr.length(), ' ');
        std::copy(wstr.begin(), wstr.end(), str.begin());
        return str;
    }

    // string 转 wstring
    static wstring s2ws(const string& s){
        // std::wstring wstr(str.length(),L' ');
        // std::copy(str.begin(), str.end(), wstr.begin());
        // return wstr;
        setlocale(LC_ALL, "ZN_CH.UTF-8"); 
        const char* _Source = s.c_str();
        size_t _Dsize = s.size() + 1;
        wchar_t *_Dest = new wchar_t[_Dsize];
        wmemset(_Dest, 0, _Dsize);
        mbstowcs(_Dest,_Source,_Dsize);
        std::wstring result = _Dest;
        delete []_Dest;
        setlocale(LC_ALL, "C");
        return result;
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
    // wstring 转 long
    static long ws2l(const wstring& str){
        return s2l(ws2s(str));
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
    // wstring 转 double
    static long ws2d(const wstring& str){
        return s2d(ws2s(str));
    }

    // 替换所有子字符串
    static void replace_all(string & s, const string & t, const string & w)
    {
        string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();  
        while(pos != string::npos){ // found   
            s.replace(pos, t_size, w);   
            pos = s.find(t, pos + r_size );   
        }  
    }

    // 分割字符串
    static void split(const string & str, string const & t, vector<string> & slist)
    {

        size_t stsz = t.size();
        size_t sta = 0;
        while(1){
            size_t found = str.find(t, sta);  
            if(found==string::npos){
                slist.push_back( str.substr(sta) );
                break;
            }
            slist.push_back( str.substr(sta, found-sta) );
            sta = found + stsz; // 更新查找位置
        }
    }


	// 去除首尾空格
	static void ltrim(string & ss) 
	{ 
		int (*pf)(int)=isspace; 
		string::iterator   p=find_if(ss.begin(),ss.end(),not1(ptr_fun(pf))); 
		ss.erase(ss.begin(),p);
	} 
	static void rtrim(string & ss) 
	{ 
		int (*pf)(int)=isspace; 
		string::reverse_iterator  p=find_if(ss.rbegin(),ss.rend(),not1(ptr_fun(pf))); 
		ss.erase(p.base(),ss.end());
	} 
	static void trim(string & st) 
	{
		rtrim(st);
		ltrim(st);
	} 






}; // --end-- class Str

} // --end-- namespace util
} // --end-- namespace def


#endif
// --end-- DEF_UTIL_STR_H