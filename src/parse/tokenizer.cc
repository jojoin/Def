/**
 * 词法分析器
 * 
 * 
 */


///////
#include <iostream>
#include <time.h>
#include <cstdlib>
///////

#include <fstream> 
#include <string>
//#include <exception>

#include "tokenizer.h"

using namespace std;

using namespace def::util;


namespace def {
namespace parse {
	
// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);

// Token::State
#define S Token::State


/**
 * 构造
 * @isFile 是否为本地文件路径
 */
Tokenizer::Tokenizer(string &text):
	filepath(""),
	text(text)
{
	// 末尾加上换行 用于兼容
	// text += "\n\n\n";
	
	words = new vector<Word>(1024);

	// 清理 初始化数据
	Clear();
}


/**
 * 保存当前单词 清空缓存
 */
void Tokenizer::Push(S sta=S::Normal)
{

	// 判断是整形还是浮点
	if(sta==S::Number){
		if(Token::IsFloat(buf)){
			sta = S::Float;
		}else{
			sta = S::Int;
		}
	// 忽略块字符串开头及末尾的两个换行
	}else if(sta==S::BlockQuotation){

		if(buf[0]=='\n'){
			buf.erase(0, 1);  
		}
		size_t len = buf.size();
		if(buf[len-1]=='\n'){
			buf.erase(len-1, 1);  
		}
		sta = S::String;
	}

	struct Word wd = {
		line_start?line_start:line,
		word_pos,
		sta,
		buf
	};

	if(sta==S::Space ){
		wd.value = "";
	}else{
		buf = "";
		word_pos++;
		line_start = 0;
	}

	words->push_back(wd);

};



/**
 * 扫描
 */
vector<Word>* Tokenizer::Scan()
{


#define IS_SIGN(str) tok==str&&s==S::Sign

    // cout << "void Tokenizer::Scan()" << endl;

	// 清理
	Clear();

	// 状态机状态

	S ss = S::Normal;
	//Token::State status = Token::State::Normal;

    //cout << text << endl;
	//return;

	while(1){

    	Read(); //读取一个字符

		// 当前字符状态
    	S s = Token::GetState(tok);
    	//cout << tok << endl;

		if(s==S::End){ // 结束
			Push(S::End);
			break; //结束
		}

		// 处理默认情况
		if(ss==S::Normal){

			if( s==S::Normal || s==S::Space || s==S::NewLine ){

				ss = S::Normal; // 忽略

			}else if(s==S::Number || s==S::Sign){

				Buf();
				ss = s;

			}else if(s==S::Character){

				Buf();
				ss = S::Character; // 标识符

			}else if(s==S::Annotation){ // 注释

				if(Peek(1)=="-"&&Peek(2)=="-"){
					Jump(2);
					ss = S::BlockAnnotation; // 块注释
				}else{
					ss = S::Annotation;
				}

			}else if(s==S::DQuotation){

				if(Peek(1)=="\""&&Peek(2)=="\""){
					Jump(2);
					line_start = line; //记录开始行
					ss = S::BlockDQuotation;
				}else{
					ss = S::DQuotation;
				}

			}else if(s==S::Quotation){

				if(Peek(1)=="\'" &&Peek(2)=="\'"){
					Jump(2);
					line_start = line; //记录开始行
					ss = S::BlockQuotation;
				}else{
					ss = S::Quotation;
				}

			}else if(s==S::Unknow){

				// 未识别的符号
				ERR(" no sign ！");

			}

#define IF_PUSH_CALL                                 \
	if(s==S::Sign){                                  \
		if(tok=="("){                                \
			Push(S::FuncCall); /*函数调用*/          \
		}else if(tok=="{"){                          \
			Push(S::ProcCall); /*处理器调用*/        \
		}else if(tok=="["){                          \
			Push(S::ContainerAccess); /*容器访问*/   \
		}                                            \
	}

		// 标识符
		}else if(ss==S::Character){

			if(s==S::Character||s==S::Number){
				Buf();
			}else{
				if(Token::IsKeyword(buf)){// 关键字？
					Push(S::Keyword);
				}else{
					Push(S::Variable); //变量名
					IF_PUSH_CALL // !访问和调用!
				}
				Back(); // 回退
				ss = S::Normal;
			}

		// 符号
		}else if(ss==S::Sign){

			if(s==S::Sign&&Token::IsSign(buf+tok)){
				Buf(); // 多元操作符
			}else{
				string bf = buf;
				// cout<<"bf =  "<<bf<<endl;
				Push(S::Sign);
				// 是否连续调用
				if(bf==")"||bf=="}"||bf=="]"){
					// cout<<" string bf = buf; "<<endl;
					IF_PUSH_CALL // !访问和调用!
				}
				Back(); // 回退
				ss = S::Normal;
			}

#undef IF_PUSH_CALL

		// 数字
		}else if(ss==S::Number){

			if(s==S::Number||tok=="."){
				Buf();
			}else if(s==S::Character){
				//TODO:: 【错误】数字后面不能跟字母
				ERR("T0002");
			}else{
				Push(S::Number);
				Back(); // 回退
				ss = S::Normal;
			}

		}else if(ss==S::Annotation){ // 注释

			if(s==S::NewLine){
				ss = S::Normal;  // 注释结束
			}

		}else if(ss==S::BlockAnnotation){ // 块注释

			if(tok=="#"&&prev_tok=="-"&&pprev_tok=="-"){
				ss = S::Normal;  // 块注释结束
			}

		}else if(ss==S::DQuotation){ //双引号字符串

			if(s==S::DQuotation){
				Push(S::String);
				ss = S::Normal; // 字符串结束
			}else if(tok=="\\"){//转义
				Read();
				Buf(Token::GetEscapeChat(tok));
			}else{
				Buf();
			}

		}else if(ss==S::Quotation){ //单引号字符串

			if(s==S::Quotation){
				Push(S::String);
				ss = S::Normal; // 字符串结束
			}else if(tok=="\\"){//转义
				Read();
				Buf(Token::GetEscapeChat(tok));
			}else{
				Buf();
			}

		}else if(ss==S::BlockDQuotation){ //块字符串

			if(tok=="\""&&prev_tok=="\""&&pprev_tok=="\""){
				Pop(2); //跳过引号
				Push(S::BlockQuotation);
				ss = S::Normal; // 块字符串结束
			}else{
				Buf();
			}

		}else if(ss==S::BlockQuotation){ //块字符串

			if(tok=="\'"&&prev_tok=="\'"&&pprev_tok=="\'"){
				Pop(2); //跳过引号
				Push(S::BlockQuotation);
				ss = S::Normal; // 块字符串结束
			}else{
				Buf();
			}

		}else{

			// 未识别的符号
			ERR(" no match sign ");
		}

    	// 换行纪录
		if(s==S::NewLine){
			//cout<<"line: "<<line<<endl;
			line++; //新行
			word_pos = 1;
			//cout<<"prev_tok: "<<prev_tok<<endl;

		}

	}

    // cout << "void Tokenizer::Scan() end" << endl;

	// 语法分析执行完毕

#undef IS_SIGN

	return words;
}


#undef S // Token::State
#undef ERR // Log::log exit


} // --end-- namespace parse
} // --end-- namespace def