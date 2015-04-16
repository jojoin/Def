#ifndef DEF_ERRORDEF_H
#define DEF_ERRORDEF_H

/**
 * 各种类型的错误定义
 *
 *
 */


#include <string>


using namespace std;


namespace def {
namespace error {


#define ET ErrorType

// 错误定义
class ErrorDef{

	public:

	static string Msg(ET type, int code)
	{
		if(type==ET::System){

		switch(code){
			case 1: return "Internal fail cannot continue.";
		}

		}else if(type==ET::Find){

		}else if(type==ET::Token){

		}else if(type==ET::Node){

		}else if(type==ET::Run){

		}

		return "";
	}


}; // --end-- class ErrorDef

#undef ET

} // --end-- namespace error
} // --end-- namespace def







#endif
// --end-- DEF_ERRORDEF_H
