
@echo off
echo - - - - - - - - - - - - - - - - - - - - - -
echo - Compile def vm c++ source code to def.exe
echo - - - - - - - - - - - - - - - - - - - - - -

cd ..\src

set "cc0=.\util\path.cc "

set "cc1=.\parse\node.cc .\parse\nodezer.cc .\parse\token.cc .\parse\tokenizer.cc"

set "cc2=.\object\object.cc .\object\operat.cc"

set "cc3=.\vm\exec.cc .\vm\gc.cc .\vm\stack.cc .\vm\module.cc"


g++ .\def.cc %cc0% %cc1% %cc2% %cc3% -std=c++11 -static -o ..\build\def.exe

cd ..\build

Rem pause


