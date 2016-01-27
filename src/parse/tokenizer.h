#pragma once

#include <fstream>
#include <iostream>
#include <string>


// 保留字符定义
#define DEF_RESERVED_TOK_SIGN '`'
#define DEF_RESERVED_TOK_STR '"'


namespace def {
namespace parse {

using namespace std;



/**
 * 词法分析器
 */
class Tokenizer
{

public:
    // 单词状态
    enum class State {
        Normal,                 // 默认
        Character,              // 英文字母（变量名和关键字）
        Number,                 // 数字
        Sign,                   // 标记
        Operator,               // 操作符
          QuoteOperator,           // 引用操作符
        Char,                   // 字符 '
        String,                 // 字符串 "
        Annotation,             // 单行注释
        BlockAnnotation,        // 块注释
        Space,                  // 空格、tab等制表符
        NewLine,                // 换行
        Unknow,                 // 不明字符
        End                     // 结束符
    };
    // 单词
    struct Word {
        State state;
        string value;
        string str();
        Word(State s=State::Unknow, const string &v="")
            : state(s)
            , value(v)
        {}
        bool inline operator==(const Word &w){
            return state == w.state && value == w.value;
        };
    }; // 缓存的
    
    
public:
    
    string file;  // 文件

    size_t cursor; // 当前游标
    size_t curline; // 当前行号

private:

    ifstream fin;  // 文件读入流

    void readpart(); // 读取文件的一部分
    string part; // 文件部分缓存
    size_t part_seek; // 读取游标
    bool finish; // 文件是否读取完毕
    
    inline void seek(int s = 1) {
        cursor += s;
        part_seek += s;
    }

    inline char escape(const char & t); // 获取转义
    inline char getchar(); // 读取一个字符，并移动游标

public:
    
    Tokenizer(const string & filepath, bool checkfile=true); // 初始化
    ~Tokenizer() {
    }
    Word gain(); // 读取一个单词
    void jumpWhitespace(); // 跳过空白字符

public:

    static State state(const char & t); // 获取单个字符的类型
    static bool isoperator(const string & tok);
    static bool isoperator(const char & t);
    static bool isfloat(const string &); // 判断是否为浮点数

};


} // --end-- namespace parse
} // --end-- namespace def
