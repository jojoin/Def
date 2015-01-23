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
#include <exception>

#include "tokenizer.h"
#include "../Util/log.h"

using namespace std;

using namespace def::token;
using namespace def::util;

// Log::log
#define ERR(str) cerr<<str<<endl;;exit(1);

// Token::State
#define S Token::State

/**
 * 构造
 * @isFile 是否为本地文件路径
 */
Tokenizer::Tokenizer(string txt, bool isFile=false):
	text(txt)
{
	// 读取本地文件
	if(isFile){
		//std::locale::global(std::locale("")); //设置语言环境，支持中文路径
		//setlocale(LC_ALL,"Chinese-simplified"); 
		ifstream inFile;
    	inFile.open(txt);
    	if(!inFile){
    		ERR("S0001")
    	}else{
    		text = "";
			string str;
			while(!inFile.eof() && getline(inFile, str))
			{
				//cout<<str<<endl;
				text += str+'\n';
			}
			//ERR("text:  "+text)
    	}
	}

	// 清理 初始化数据
	Clear(); 
}


/**
 * 保存当前单词 清空缓存
 */
void Tokenizer::Push(S sta=S::Normal)
{
	if(buf==""){
		return;
	}

	// 判断是否为关键字
	if(sta==S::Identifier
	   && Token::IsKeyword(buf)
	){ 
		sta = S::Keyword;
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
		line,
		pos,
		sta,
		buf
	};
	words.push_back(wd);
	buf = "";
};



/**
 * 扫描
 */
vector <Word> & Tokenizer::Scan()
{

	// 清理
	Clear();

	// 状态机状态

	S ss = S::Normal;
	//Token::State status = Token::State::Normal;

	while(1){

    	Read(); //读取一个字符

		// 当前字符状态
    	S s = Token::GetState(tok);

    	// 换行
		if(s==S::NewLine){
			line++; //新行
			pos = 0;
		}
		pos++;  //字符位置

		//Log::log(tok);

		//cout<<"State"<<endl;

		if(ss==S::Normal){

			if(s==S::Normal){
				Push();
			}else if(s==S::Number){
				Buf();
				ss = S::Number;
			}else if(s==S::Annotation){
				ss = S::Annotation;
			}else if(s==S::Character){
				Buf();
				ss = S::Character;
			}else if(s==S::Sign){
				Buf();
				ss = S::Sign;
			}else if(s==S::NewLine){ // 
				//Buf();
				//Push(S::NewLine);
				//ss = S::Normal;
			}else if(s==S::End){ // 结束
				Push(S::End);
				break;
			}

		}else if(ss==S::Space){ // 空白符 忽略


		}else if(ss==S::Annotation){ // 注释 忽略

			if(s==S::NewLine){
				ss = S::Normal;
			}

		}else if(ss==S::Number){ // 数字

			if(s==S::Number||tok=='.'){
				Buf();
			}else if(s==S::Character){
				//TODO:: 【错误】数字后面不能跟字母
				ERR("T0002");
			}else{
				Push(S::Number);
				Back();
				ss = S::Normal;
			}

		}else if(ss==S::Character){ // 字母

			if(s==S::Character||s==S::Number){
				Buf();
			}else{
				Push(S::Identifier);
				Back();
				ss = S::Normal;
			}

		}else if(ss==S::Sign){ // 有效符号

			if(s==S::Sign){
				Buf();
				ss = S::Sign;
			}else{
				Push(S::Sign);
				Back();
				ss = S::Normal;
			}

		}else if(ss==S::NewLine){ // 换行
			//连续换行将被忽略
			
		}else if(ss==S::End){ // 结束


			Push(S::End);
			//TODO:: 不是普通状态的结束 错误
			break;
			//cout<<123145<<endl;
		}


	}

	return words;
}



#undef S // Token::State
#undef ERR // Log::log exit





/*******************  单元测试   ******************/


int main()
{
    cout << "\n";

    Tokenizer TK("./Parser/test.d", true);
    vector <Word> words = TK.Scan();

    for(int i=0; i<words.size(); i++){
    	Word wd = words[i];
		cout << wd.line << ","<< wd.pos << "  " << (int)wd.type << "  " << wd.value << endl;
    }



	//wcout << L"1: " << words[0] << endl;


	//std::locale::global(std::locale("zh_CN.UTF-8"));
	//setlocale(LC_ALL ,"zh_CN.UTF-8"); //显示中文
	//wcout.imbue(locale("chs"));
	//wcout.imbue(locale("zh_CN.UTF-8")); 


	//wcout << L"txt: " << L"13 + x*2 //注释";
	//wcout << L"txt: " << ttt.length() << endl;
	//wcout << L"txt: " << tok << endl;
	//wprintf(L"\nkkk: %s", L"13 + x*2 //注释");
	//wprintf(L"\nkkk: %s", ttt);
    //cout << "num: "<<Token::keywords[0];


    cout << "\n\n";
    //int end;
    //cout << "\n\n--- Enter Anything To Exit ---";
    //cin >> end;
}


/*******************  测试结束   ******************/
