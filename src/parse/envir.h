#pragma once


/** 
 * Def 语法分析环境
 */


#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "../global.h"

#include "./element.h"
#include "../core/ast.h"




namespace def {
namespace parse {

using namespace std;
using namespace def::core;
// using namespace def::util;
// using namespace def::parse;
// using namespace def::compile;



#define ElementStack map<string, Element*>



/**
 * Def 语法分析环境
 */

struct Envir
{
    Envir* parent = nullptr; // 父环境
    
    ElementStack stack;     // 变量，函数，类型，函数模板分析栈
    
    ASTFunctionDefine* wrap = nullptr; // 外层函数


};





}

}