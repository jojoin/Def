@echo off

::   Debug   Release

:: -d ast_tree,als_stack --print ir 
I:\GitHub\Def\x64\Debug\def "index.def" -d ast_tree,als_stack --print ir --emit obj -o index.obj

:: -d ast_tree,als_stack,tok_list     打印 AST，分析栈，token字符（仅Debug版本可用）
:: -d prepare_words,binding_spread,mulmcr_words   打印宏展开结果
:: --print ir                    打印 LLVM IR
:: --emit ast -o index.txt       输出 AST 至 文件 index.txt
:: --emit ir -o index.ll         输出 LLVM IR 至 文件 index.ll       
:: --emit asm -o index.s         输出 汇编 至 文件 index.s
:: --emit obj -o index.obj       输出 object 至 文件 index.obj

:: LLVM 相关编译
:: llc index.ll 
:: llc -filetype=obj index.ll

:: gcc 编译对象文件至目标代码
:: gcc index.s -o index.exe
gcc index.obj "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib\chkstk.obj" -o index.exe


:: 运行 Def 编译结果（可执行文件）
echo --------
index.exe
echo.
echo --------

