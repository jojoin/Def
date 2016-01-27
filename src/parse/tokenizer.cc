/**
 * 词法分析器
 */

#include <codecvt>

#include "tokenizer.h"

#include "../core/error.h"
#include "../util/fs.h"
#include "../util/str.h"
#include "../sys/debug.h"

using namespace std;
using namespace def::util;
using namespace def::sys;
using namespace def::core;


namespace def {
namespace parse {


#define ERR(str) cerr<<str<<endl;exit(1);

#define BUFFER_SIZE 1024 // 文件读取 buffer 大小

  

/**
 * Word 打印
 */
string Tokenizer::Word::str()
{
    if (state==State::String) {
        return "\"" + value + "\"";
    }
    if (state==State::Char) {
        return "'" + value + "'";
    }
    if (state==State::QuoteOperator) {
        return "`" + value + "`";
    }
    return value;
}



/**
 * 构造
 */
Tokenizer::Tokenizer(const string & f, bool checkfile):
    cursor(0),
    curline(1),
    part_seek(0),
    finish(false),
    part(""),
    file(f)
{
    if (checkfile && !Fs::exist(f)) {
        cout<<"Cannot find file '"+file+"' !\n";
        std::exit(0);
    }

    // 调试打印
    DEBUG_COUT("include", "[include] "+f)
    
    fin.open(f);

    // fin = ifstream(f);

    // 预读一部分
    readpart();
    // 切换错误报告信息
    Error::update(this);
}




/**
 * 跳过空白字符
 */
void Tokenizer::jumpWhitespace()
{
    char c;
    while (1) {
        c = getchar();
        if (c=='#') { // 注释
            c = getchar();

            if ('-'==c) { // 块
                char o = getchar();
                while (1) {
                    c = getchar();
                    if (o=='-' && c=='#') {
                        break; // 块结束
                    }
                    o = c;
                }

            } else {
                while (1) {
                    if (c=='\n') {
                        break; // 行结束
                    }
                    c = getchar();
                }
            }
            continue;
        }
        if (c!=' '&&c!='\n'&&c!='\t') {
            break;
        }
    }
    seek(-1);
}



/**
 * 读取文件的一部分
 */
void Tokenizer::readpart()
{
    char buf[BUFFER_SIZE+1];
    if (fin.get(buf, BUFFER_SIZE+1, '\0')) {
        part = string(buf);
    }
    else {
        part = ""; // 读完
    }
    part_seek = 0;
}

/**
 * 取得一个字符并移动游标
 */
char Tokenizer::getchar()
{
    if (finish) { // 已经读取结束
        return '\0';
    }
    if (part_seek==BUFFER_SIZE) {
        readpart(); // 再读
    }
    if(part==""){
        return '\0'; // 结束
    }
    /*
    if (part_seek>=part.size()) {
        return '\0'; // 结束
    }
    */
    // 读取单个字符
    char t = part[part_seek];
    seek(1); // 向前一步
    if(t=='\n'){
        curline++; // 换行
        cursor = 0;
    }
    return t;

}


/**
 * 获取转义字符
 */
char Tokenizer::escape(const char & t)
{
    switch (t) {
        case '\'':  // fall through
        case '"' :  // fall through
        case '\\': 
        return t;
        case '0' : return '\0'; //NULL空字符
        case 'a' : return '\a'; //响铃
        case 'b' : return '\b'; //退格
        case 'f' : return '\f'; //换页
        case 'n' : return '\n'; //换行
        case 'r' : return '\r'; //回车
        case 't' : return '\t'; //水平制表
    }
    return t;
}


/**
 * 判断是否为合法的符号
 */
bool Tokenizer::isoperator(const char & t)
{
    // 保留符号 # () ' " ` ;
    string opts = "~!@$%^&=+-*/,.:?|<>[]{}\\";
    int pos = opts.find_first_of(t);
    return pos==-1 ? false : true;
}
bool Tokenizer::isoperator(const string & tok)
{
    int len = tok.length();
    for(int i=0; i<len; i++){
        if(!isoperator(tok[i])){
            return false;
        }
    } 
    return true;
}


/**
 * 获取单个字符的类型
 */
Tokenizer::State Tokenizer::state(const char & tok)
{
    if( (tok>='a' && tok<='z') || 
        (tok>='A' && tok<='Z') ||
        tok=='_'
    ){ //字母或下划线
        return State::Character;
    }

    if( tok==' ' || tok=='\t'){ // 空格或tab
        return State::Space;
    }

    if( tok>='0' && tok<='9' ){ // 数字
        return State::Number;
    }

    if( tok=='#' ){ // 注释
        return State::Annotation;
    }
    
    if( tok=='"' ){  // 双引号 字符串
        return State::String;
    }
    if( tok=='\'' ){ // 单引号 字符
        return State::Char;
    }

    if (tok == '(' || tok == ')' || tok == ';') { // 标记
        return State::Sign;
    }

    if( tok==DEF_RESERVED_TOK_SIGN ){ // 符号引用
        return State::QuoteOperator;
    }

    //有效符号
    if( isoperator(tok) ){
        return State::Operator;
    }

    if( tok=='\n' ){ //结束
        return State::NewLine;
    }

    if( tok=='\0' ){ //结束
        return State::End;
    }

    return State::Unknow;


}



#define S State
#define OS(T) os==S::T
#define TS(T) ts==S::T
#define RESET(T) ts = S::T;
#define UP(T) os = S::T; continue;
#define BUF() curword.append(1,t); continue;
#define RET(T) \
    if(TS(Character)||TS(Number)||TS(Sign)\
    ||TS(Operator)||TS(Char)||TS(String))\
    { seek(-1); } \
    return Tokenizer::Word{ Tokenizer::State::T, curword };


/**
 * 取得一个单词并移动游标
 */
Tokenizer::Word Tokenizer::gain()
{

    char prev(' ');
    string curword("");

    S os = S::Normal;

    while (1) {

        char t = getchar();
        S ts = state( t );

        // 文件读取结束
        if(TS(End)){
            finish = true; // 结束标记
            if (curword!="") { // 返回最后一个token
                return Tokenizer::Word( os, curword );
            }
            // 返回结束
            return Tokenizer::Word(
                Tokenizer::State::End, "");
        }

        // 开始
        if(OS(Normal)){

            // 无需缓存的
            if(TS(NewLine)||TS(Space)){
                continue;
            } else if (TS(Annotation)) { // 注释
                char ct = getchar();
                if (ct == '-') {
                    UP(BlockAnnotation)
                } else if (ct == '\n') {
                    UP(Normal) // 单行注释结束
                } else {
                    UP(Annotation)
                }
            } else if (TS(Char)
                ||TS(String)
                ||TS(QuoteOperator)) {
                os = ts; // 切换状态
                continue; 
            }
            // 需要缓存的
            os = ts;
            BUF()

        // 注释
        } else if (OS(Annotation)){
            if(TS(NewLine)){
                UP(Normal) // 单行注释结束，恢复状态
            }

        // 块注释
        } else if (OS(BlockAnnotation)){
            char ct = getchar();
            while(1){
                if(prev=='-'&&ct=='#'){
                    break; // 块注释结束
                }
                prev = ct;
                ct = getchar();
            }
            UP(Normal) // 恢复状态


        // 名字
        } else if (OS(Character)){
            if(TS(Character)||TS(Number)){
                BUF()
            }
            RET(Character)

        // 数字
        } else if (OS(Number)){
            if( TS(Number) || t=='.' ){
                BUF()
            }
            RET(Number)

        // 符号
        } else if (OS(Sign)){
            RET(Sign)

        // 操作符
        } else if (OS(Operator)){
            if (TS(Operator)) {
                BUF()
            } else { // 结束
                RET(Operator)
            }




        // 引用操作符（将单词变为操作符）
        } else if (OS(QuoteOperator)){
            if(t=='\\'){ // 转义
                curword.append(1, escape( getchar() ) );
                continue;
            }else if( TS(NewLine) ){
                FATAL("Quote Sign Missing End !")
            }else if(TS(QuoteOperator)){
                RET(Operator)
            }
            BUF()

        // 单字符
        } else if (OS(Char)){
            // 允许保存多个 char 兼容 UTF8 编码
            if (TS(Char)) { // 结束
                RESET(Normal) // 复位
                RET(Char)
            }
            if(t=='\\'){ // 转义
                t = escape( getchar() );
            }
            BUF()

        // 双字符串
        } else if (OS(String)){
            if(t=='\\'){ // 转义
                curword.append(1, escape( getchar() ) );
                continue;
            }else if(TS(String)){
                RESET(Normal) // 复位
                RET(String)
            }
            BUF()
        }
    }
}


#undef  S
#undef OS
#undef TS
#undef BUF
#undef RET




//判断是否为浮点数
bool Tokenizer::isfloat(const string & str)
{
    // npos 表示未找到
    if(str.find(".")!=string::npos){
        return true;
    }
    return false;
}






} // --end-- namespace parse
} // --end-- namespace def


