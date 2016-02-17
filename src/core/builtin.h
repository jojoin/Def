#pragma once



// 内建语言核心定义
#define BUILTIN_DEFINES_LIST(D) \
    D(include)    /*文件包含*/ \
    D(namespace)  /*名字空间*/ \
    \
    D(let) /*符号绑定*/ \
    \
    D(type)/*类型定义*/ \
    D(dcl) /*函数声明*/ \
    D(fun) /*函数定义*/ \
    D(ret) /*函数返回*/ \
    D(tpf) /*模板函数定义*/ \
    D(var) /*变量定义*/ \
    D(set) /*变量赋值*/ \
    \
    D(if) /*if else*/ \
    D(while) /*while*/ \
    \
    D(elmget) /*成员访问*/ \
    D(elmset) /*成员赋值*/ \
    D(elmivk) /*成员函数调用*/ \
    D(elmdef) /*成员函数外部定义*/ \
    \
    D(mcrfor) /*宏重复展开*/ \
    D(mcrif)  /*宏条件展开*/ \
    
  

// 内建函数（第一个类型为函数返回值类型，剩余的为参数类型）
#define BUILTIN_FUNCTION_LIST(B,C) \
    /* 内建函数 */ \
    B(bool, "Bool,Bool") \
    B(bool, "Bool,Int") \
    B(add, "Int,Int,Int") \
    B(sub, "Int,Int,Int") \
    /* C 标准库函数 */ \
    /* math.h */ \
    C(abs, "Int,Int") \
    /* stdio.h */ \
    C(getchar, "Int") \
    C(putchar, "Int,Int") \
    C(getchar, "Char") \
    C(putchar, "Char,Char") \
    C(gets,    "String") \
    C(puts,    "Int,String") \


