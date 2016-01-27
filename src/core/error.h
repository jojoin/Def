#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "../parse/tokenizer.h"


namespace def {
namespace core {
    
using namespace std;
using namespace def::parse;


/**
* 错误报告
*/
class Error
{

public:
    // 调用栈结构
    struct Position {
        size_t line;   // 所在行
        size_t cursor; // 游标位置
    };

public:
    
    static Tokenizer* tkz;  // 当前分析器
    static void update(Tokenizer* t) { 
        tkz = t;
    }

    // 错误报告并终止程序
    static void exit(const string &, const string & f="");
    static string getFoucsLine(); // 焦点行及游标位置

    // 调用栈操作
    static Position createPosition(); // 建立快照
    static Position curPosition(); // 当前快照
    static void snapshot();  // 建立并保存快照
    static void backspace(int n=1); // 回退

private:

    // 快照记录
    static vector<Position> positions;

};


}
}



// 普通错误
#define ERR(str) cerr<<str<<endl;
// 致命错误
#define FATAL(str) cerr<<endl<<endl<<str<<endl; exit(1);
