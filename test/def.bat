@echo off

::   Debug   Release

:: -d ast_tree,als_stack --print ir 
I:\GitHub\Def\x64\Debug\def "index.def" -d ast_tree,als_stack --print ir --emit obj -o index.obj

:: -d ast_tree,als_stack,tok_list     ��ӡ AST������ջ��token�ַ�����Debug�汾���ã�
:: -d prepare_words,binding_spread,mulmcr_words   ��ӡ��չ�����
:: --print ir                    ��ӡ LLVM IR
:: --emit ast -o index.txt       ��� AST �� �ļ� index.txt
:: --emit ir -o index.ll         ��� LLVM IR �� �ļ� index.ll       
:: --emit asm -o index.s         ��� ��� �� �ļ� index.s
:: --emit obj -o index.obj       ��� object �� �ļ� index.obj

:: LLVM ��ر���
:: llc index.ll 
:: llc -filetype=obj index.ll

:: gcc ��������ļ���Ŀ�����
:: gcc index.s -o index.exe
gcc index.obj "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib\chkstk.obj" -o index.exe


:: ���� Def ����������ִ���ļ���
echo --------
index.exe
echo.
echo --------

