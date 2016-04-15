#pragma once



// 内建语言核心定义
#define BUILTIN_DEFINES_LIST(D) \
    D(include)    /*文件包含*/ \
    D(scope) /*声明名字空间*/ \
    D(uscp) /*使用名字空间*/ \
    D(delscp) /*卸载取消使用名字空间*/ \
    \
    D(var) /*变量定义*/ \
    D(set) /*变量赋值*/ \
    D(type)/*类型定义*/ \
    D(tydef)/*类型重命名*/ \
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
    D(refer) /*引用类型*/ \
    D(array) /*数组类型*/ \
    D(tuple) /*元组类型*/ \
    /*D(struct) /*结构类型*/ \
    \
    D(arrget) /*数组成员访问*/ \
    D(arrset) /*数组成员赋值*/ \
    /*D(arrrfd) /*判断引用类型是否已经赋值（非0）返回Bool*/ \
    \
    D(elmget) /*类成员访问*/ \
    D(elmset) /*类成员赋值*/ \
    /*D(elmrfd) /*判断引用类型是否已经赋值（非0）返回Bool*/ \
    D(elmivk) /*类成员函数调用*/ \
    D(elmdef) /*类成员函数外部定义*/ \
    \
    D(mcrif)  /*宏条件展开*/ \
    D(mcrfor) /*宏重复展开*/ \
    D(mcrcut) /*宏分段切开*/ \
    D(mcrlnk) /*宏连接两个词*/ \
    \
    D(uvnnew) /*全局唯一名称定义*/ \
    D(uvnget) /*全局唯一名称获取 */ \
    D(uvndel) /*全局唯一名称删除*/ \
    D(uvnclear) /*全局唯一名称清空*/ \
    

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