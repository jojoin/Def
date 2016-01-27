

// 是否为调试打印模式

/* DEBUG_WITH 函数参数解析

   tok_list         打印词法分析序列
   ast_tree         打印最终 AST tree
   als_stack        打印语法分析栈帧
   include          打印 include 文件列表
   repeat_include   重复包含并忽略的文件列表

*/





/*********    *********/

// #define DEF_LETBIND_SPX " " // let 操作符绑定分隔符
// #define DEF_FuncPREV_SIGN '_' // 函数参非预求职标记
// 函数 call 类型 
// #define DEF_CALLTYPE_CORE 'c' // 系统内建
// #define DEF_CALLTYPE_USER 'u' // 用户定义
// #define DEF_CALLTYPE_FUNC 'f' // 用户定义（默认对函数参求值）



// 判断是否为window 

#ifdef _WIN32
    #define WINDOWS 1
#endif
#ifdef WIN32
    #define WINDOWS 1
#endif
#ifdef _WIN64
    #define WINDOWS 1
#endif
#ifdef WIN64
    #define WINDOWS 1
#endif
