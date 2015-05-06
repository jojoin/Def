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

using namespace def::token;
using namespace def::util;

// Log::log
#define ERR(str) cerr<<str<<endl;exit(1);

// Token::State
#define S Token::State

/**
 * 构造
 * @isFile 是否为本地文件路径
 */
Tokenizer::Tokenizer(bool isFile, string txt, vector<Word>* wds):
	filepath(""),
	text(txt),
	words(wds)
{
	// 读取本地文件
	if(isFile){

		// 一次性读取文件所有内容至string
		// cout << "isFile" << endl;
		ifstream in(txt, ios::in);
		// cout << "ifstream" << endl;
		istreambuf_iterator<char> beg(in), end;
		// cout << "istreambuf_iterator" << endl;
		string strdata(beg, end);
		// cout << "strdata" << endl;
		in.close();
		// cout << "close" << endl;
		text = strdata;
		// cout << text << endl;


		/*
		//std::locale::global(std::locale("")); //设置语言环境，支持中文路径
		//setlocale(LC_ALL,"Chinese-simplified"); 
		cout << "isFile" << endl;

		ifstream file;

		cout << "ifstream" << endl;

    	file.open(txt);

		cout << "file.open" << endl;

    	if(!file){
    		ERR("打开文件\""+txt+"\"出错！");
    	}else{
			cout << "getline" << endl;
    		getline(file, text, '\0');
    	}

    	filepath = txt;

    	file.close();
    	
    	//getcwd();
    	*/

	}

	// 末尾加上换行 用于兼容
	text += "\n\n\n";

	// 清理 初始化数据
	Clear(); 
}


/**
 * 保存当前单词 清空缓存
 */
void Tokenizer::Push(S sta=S::Normal)
{
	if( sta!=S::NewLine && buf==""){
		return;
	}
	// 判断是否为关键字
	if(sta==S::Identifier){
		if(Token::IsKeyword(buf)){
			sta = S::Keyword; //关键字
		}else{
			sta = S::Symbol; //变量名
		}
	}

	// 判断是整形还是浮点
	if(sta==S::Number){
		if(Token::IsFloat(buf)){
			sta = S::Float;
		}else{
			sta = S::Int;
		}
	}

	struct Word wd = {
		line_start?line_start:line,
		word_pos,
		sta,
		buf
	};

	words->push_back(wd);

	line_start = 0;
	buf = "";
	word_pos++;
};



/**
 * 扫描
 */
void Tokenizer::Scan()
{

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

    	//cout << "Read();" << endl;
    	//cout << tok << endl;

		//Log::log(tok);

		//cout<<"State"<<endl;

		if(ss==S::Normal){

			if(s==S::Normal){
				Push();
			}else if(s==S::Number){
				Buf();
				ss = S::Number;
			}else if(s==S::Annotation){//注释
				if(Peek()=="#"&&Peek(2)=="#"){
					//块注释
					do{ Jump(); }while(Peek()!="\n");
					ss = S::BlockAnnotation;
				}else{
					ss = S::Annotation;
				}
			}else if(s==S::Character){
				Buf();
				ss = S::Character;
			}else if(s==S::Sign){
				Buf();
				string next = Peek();
				if(Token::IsSign(next)&&Token::IsSign(tok+next)){
					Buf(next);
					Jump();
				}
				Push(S::Sign);
				ss = S::Normal;
			//}else if(s==S::NewLine){
				//Buf();
				//Push(S::NewLine);
				//ss = S::Normal;
			}else if(s==S::DQuotation){
				if(Peek()=="\""&&Peek(2)=="\""){
					Jump(2);
					line_start = line; //记录开始行
					ss = S::BlockDQuotation;
				}else{
					ss = S::DQuotation;
				}
			}else if(s==S::Quotation){
				if(Peek()=="\""&&Peek(2)=="\""){
					Jump(2);
					line_start = line; //记录开始行
					ss = S::BlockQuotation;
				}else{
					ss = S::Quotation;
				}
			}else if(s==S::End){ // 结束
				Push(S::End);
				break;
			}else if(s==S::Unknow){
				// 未识别的符号
				Error(1);
			}

		}else if(ss==S::Space||ss==S::NewLine){ // 空白符 忽略

				ss = S::Normal;

		}else if(ss==S::Annotation){ // 注释 忽略

			if(s==S::NewLine){
				ss = S::Normal;
			}else if(s==S::End){
				break; //结束
			}

		}else if(ss==S::BlockAnnotation){ // 块注释 忽略

			if(s==S::End){
				break;
			}else if(tok=="#"&&prev_tok=="#"&&pprev_tok=="#"){
				//块注释结束
				while(Peek()!="\n"){
					Jump();
				};
				ss = S::Normal;  // 结束
			}

		}else if(ss==S::Number){ // 数字

			if(s==S::Number||tok=="."){
				Buf();
			}else if(s==S::Character){
				//TODO:: 【错误】数字后面不能跟字母
				ERR("T0002");
			}else{
				Push(S::Number);
				if(s!=S::NewLine) Back();
				ss = S::Normal;
			}

		}else if(ss==S::Character){ // 字母

			if(s==S::Character||s==S::Number){
				Buf();
			}else{
				Push(S::Identifier);
				if(s!=S::NewLine) Back();
				ss = S::Normal;
			}

		}else if(ss==S::Sign){ // 有效符号

			if(s==S::Sign){
				Buf();
			}else{
				Push(S::Sign);
				if(s!=S::NewLine) Back();
				ss = S::Normal;
			}

		}else if(ss==S::DQuotation){ //双引号字符串

			if(s==S::DQuotation){
				Push(S::String);
				ss = S::Normal;
			}else if(tok=="\\"){//转义
				Read();
				Buf(Token::GetEscapeChat(tok));
			}else{
				Buf();
			}

		}else if(ss==S::Quotation){ //单引号字符串

			if(s==S::Quotation){
				Push(S::String);
				ss = S::Normal;
			}else if(tok=="\\"){//转义
				Read();
				Buf(Token::GetEscapeChat(tok));
			}else{
				Buf();
			}

		}else if(ss==S::BlockDQuotation){ //块字符串

			if(tok=="\""&&prev_tok=="\""&&pprev_tok=="\""){
				Pop(2); //删除引号
				Push(S::BlockDQuotation);
				ss = S::Normal;
			}else{
				Buf();
			}

		}else if(ss==S::BlockQuotation){ //块字符串

			if(tok=="\'"&&prev_tok=="\'"&&pprev_tok=="\'"){
				Pop(2); //删除引号
				Push(S::BlockQuotation);
				ss = S::Normal;
			}else{
				Buf();
			}

		}else if(ss==S::End){ // 结束

			Push(S::End);
			break; //结束
		}else{

			// 未识别的符号
			Error(1);
		}


    	// 换行
		if(s==S::NewLine){
			//cout<<"line: "<<line<<endl;
			line++; //新行
			word_pos = 1;
			//cout<<"prev_tok: "<<prev_tok<<endl;

		}

	}

    // cout << "void Tokenizer::Scan() end" << endl;

	// 语法分析执行完毕

}



#undef S // Token::State
#undef ERR // Log::log exit

