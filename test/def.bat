@echo off

::   Debug   Release
I:\GitHub\Def\x64\Release\def "index.def" --emit ir -o index.ll
::-d ast_tree,als_stack,tok_list
:: --emit obj -o index.obj 
:: --print ir
:: --emit ir -o index.ll
:: --emit asm -o index.s
:: --emit obj -o index.obj
:: -d ast_tree,als_stack,tok_list



:: llc index.ll
:: llc -filetype=obj index.ll
:: gcc index.s -o index.exe
:: gcc index.obj "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib\chkstk.obj" -o index.exe


:: echo --------
:: index.exe
:: echo;
:: echo;

