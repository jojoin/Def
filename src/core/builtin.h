#pragma once



// 内建语言核心定义
#define BUILTIN_DEFINES_LIST(D) \
    D(include)    /*文件包含*/ \
    D(namespace)  /*名字空间*/ \
    \
    D(var) /*变量定义*/ \
    D(set) /*变量赋值*/ \
    D(type)/*类型定义*/ \
    D(dcl) /*函数声明*/ \
    D(fun) /*函数定义*/ \
    D(ret) /*函数返回*/ \
    \
    D(let) /*符号绑定*/ \
    D(tpf) /*函数模板*/ \
    D(tpty) /*类模板*/ \
    \
    D(adt) /*适配器模式*/ \
    \
    D(new) /*内存申请*/ \
    D(delete) /*内存释放*/ \
    D(copy) /*拷贝对象*/ \
    \
    D(if) /*if else*/ \
    D(while) /*while*/ \
    \
    D(refer) /*类型为引用*/ \
    D(array) /*类型为数组*/ \
    /*D(struct) 类型为元组*/ \
    \
    D(arrget) /*数组成员访问*/ \
    D(arrset) /*数组成员赋值*/ \
    \
    D(elmget) /*类成员访问*/ \
    D(elmset) /*类成员赋值*/ \
    D(elmivk) /*类成员函数调用*/ \
    D(elmdef) /*类成员函数外部定义*/ \
    \
    D(mcrif)  /*宏条件展开*/ \
    D(mcrfor) /*宏重复展开*/ \
    D(mcrcut) /*宏分段切开*/ \
    D(mcrlnk) /*宏连接两个词*/ \
    

    // \
    // D(link) /*取得变量引用*/ \
    // D(load) /*从引用载入变量*/ \
  

// 内建函数（第一个类型为函数返回值类型，剩余的为参数类型）
#define BUILTIN_FUNCTION_LIST(B,C) \
    /* 内建函数 */ \
    B(bool, "Bool,Bool") \
    B(bool, "Bool,Int") \
    /* 基本算法 */ \
    B(add, "Int,Int,Int") \
    B(sub, "Int,Int,Int") \
    B(mul, "Int,Int,Int") \
    /* 条件判断 */ \
    B(eq, "Bool,Int,Int") \
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


/*
    B(eq)  /*相等* / \
    B(ne)  /*不等* / \
    B(gt)  /*大于* / \
    B(ge)  /*大于等于* / \
    B(lt)  /*小于* / \
    B(le)  /*小于等于* / \
*/