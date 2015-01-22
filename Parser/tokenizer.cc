/**
 * 词法分析器
 * 
 * 
 */


///////
#include <iostream>
#include <time.h>
///////

#include <string>
#include <exception>

#include "tokenizer.h"
#include "../Util/log.h"

using namespace std;

using namespace def::token;
using namespace def::util;


/**
 * 构造
 */
Tokenizer::Tokenizer(wstring txt):
	text(txt),
	otok(L' '),
	tok(L' '),
	ptok(L' '),
	buf(L""),
	pos(0)
{
	//

}


/**
 * 扫描
 */
vector <wstring> & Tokenizer::Scan()
{

	// 清理
	Clear();

	// 状态机状态

#define S Token::State

	S ss = S::Normal;
	//Token::State status = Token::State::Normal;

	while(1){

    	Read(); //读取一个字符

		// 当前字符状态
    	S s = Token::GetState(tok);

		//cout<<"State"<<endl;

		if(ss==S::Normal){

			if(s==S::Normal){
				PopBuf();
			}else if(s==S::Number){
				Buf();
				ss = S::Number;
			}else if(s==S::Annotation){
				ss = S::Annotation;
			}else if(s==S::Letter){
				Buf();
				ss = S::Letter;
			}else if(s==S::Sign){
				Buf();
				ss = S::Sign;
			}else if(s==S::End){ // 结束
				PopBuf();
				break;
			}

		}else if(ss==S::Annotation){ // 注释 忽略

			if(s==S::NewLine){
				ss = S::Normal;
			}

		}else if(ss==S::Number){ // 数字

			if(s==S::Number){
				Buf();
			}else{
				PopBuf();
				Back();
				ss = S::Normal;
			}

		}else if(ss==S::Letter){ // 字母

			if(s==S::Letter||s==S::Number){
				Buf();
			}else{
				PopBuf();
				Back();
				ss = S::Normal;
			}

		}else if(ss==S::Sign){ // 有效符号

			if(s==S::Sign){
				Buf();
				ss = S::Sign;
			}else{
				PopBuf();
				Back();
				ss = S::Normal;
			}

		}else if(ss==S::End){ // 结束
			PopBuf();
			//TODO:: 不是普通状态的结束 错误
			break;
		}


#undef S

	}

	return words;
}









/*******************  单元测试   ******************/


int main()
{
    wcout << L"\n";

    wstring text = L"if(a==12)#注释\n tup1 = tuple(tar get)\n\n#注释\n _count123 = 139 +num*2\n\n";
	text += text+text+text+text+text+text+text+text+text+text+text+text+text+text+text+text;
	text += text+text+text+text+text+text+text+text+text+text+text+text+text+text+text+text;
    wstring TXT = text+text+text+text;
    //TXT += text+text+text+text+text+text+text+text+text+text+text+text+text+text+text+text;



    double  tlen = TXT.length()/1024;
	wcout << "text: " << tlen << "KB" <<endl;

    int nnn = 100;

    Tokenizer TK(TXT);

    Log::startTime();

    while(nnn--){
    	TK.Scan();
    }

    Log::endTime();

    Log::log("yangjie");


/*
    Tokenizer TK(TXT);
    vector <wstring> words = TK.Scan();

    for(int i=0; i<words.size(); i++){
    	wstring ln =  L"";
    	if(i<10) ln += L" ";
    	if(i<100) ln += L" ";
		//wcout << ln << i << " : "<<words[i] << endl;
    }

*/


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


    wcout << L"\n\n";
    //int end;
    //cout << "\n\n--- Enter Anything To Exit ---";
    //cin >> end;
}


/*******************  测试结束   ******************/
