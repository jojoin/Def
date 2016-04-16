#pragma once

/**
 * 调试打印类
 */


#include <iostream>
#include <string>

#include "../global.h"


/**  调试处理  **/


namespace def {
namespace sys {

using namespace std;




class Debug
{
    static std::string type_1;
    static std::string type_2;
    static std::string type_3;
    static std::string type_4;
    static std::string type_5;

public:
    static bool CheckType(const std::string & t)
    {
        if (t == type_1
            || t == type_2
            || t == type_3
            || t == type_4
            || t == type_5) {
            return true;
        }
        return false;
    }
    static void SetType(const std::string & t, size_t n = 1) {
        if (1 == n) type_1 = t;
        if (2 == n) type_2 = t;
        if (3 == n) type_3 = t;
        if (4 == n) type_4 = t;
        if (5 == n) type_5 = t;
    }
};


}
}


// 打印指定类型的错误
#ifdef _DEBUG
#define DEBUG_WITH(T,D) \
        if(Debug::CheckType(T)){ \
            D \
        }
#define DEBUG_COUT(T,D) \
        if(Debug::CheckType(T)){ \
            cout<<D<<endl; \
        }
#define ASSERT(V,T) \
        if(!(V)){ \
            cout<<"Assert Error ("<<#V<<"): "<<T<<endl; \
            throw T; \
        }

#else
    #define DEBUG_WITH(T,D)
    #define DEBUG_COUT(T,D)
    #define ASSERT(V,T)
#endif

