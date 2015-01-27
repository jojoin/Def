
/**
 * 工具类
 */

#ifndef DEF_UTIL_LOG_H
#define DEF_UTIL_LOG_H


#include <string>
#include <map>
#include <time.h>

using namespace std;


namespace def {
namespace util {


class Log {


	public:


	// 耗时测试 开始
	inline static void startTime(string name="time"){
    	times[name] = (long)clock(); //保存开始时间
	}

	// 耗时测试 结束
	inline static void endTime(string name="time"){
	    double duration = (double)(clock() - times[name]) / CLOCKS_PER_SEC;
		cout << name << "(s): " << duration << endl;
		times.erase(name);
	}

	// 耗时测试保存时间
	static map<string, long> times;


	// 打印
	inline static void log(
		string t0="",
		string t1="",
		string t2="",
		string t3="",
		string t4="",
		string t5="",
		string t6="",
		string t7="",
		string t8="",
		string t9=""
	){
		cout<<t0<<t1<<t2<<t3<<t4<<t5<<t6<<t7<<t8<<t9<<endl;
	}

	// 打印
	inline static void log(
		wchar_t t
	){
		wcout<<t<<endl;
	}

	// 打印
	inline static void log(
		wstring t0=L"",
		wstring t1=L"",
		wstring t2=L"",
		wstring t3=L"",
		wstring t4=L"",
		wstring t5=L"",
		wstring t6=L"",
		wstring t7=L"",
		wstring t8=L"",
		wstring t9=L""
	){
		wcout<<t0<<t1<<t2<<t3<<t4<<t5<<t6<<t7<<t8<<t9<<endl;
	}




	private:

	//static clock_t start, finish;
	//static double duration;
	//保存的时间




}; // --end-- class Log

//map<string, clock_t> Log::times;



} // --end-- namespace util
} // --end-- namespace def


#endif
// --end-- DEF_UTIL_LOG_H