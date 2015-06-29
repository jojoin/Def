
@echo off
echo - - - - - - - - - - - - - - - - - - - - - -
echo - Compile def vm c++ source code to def.exe
echo - - - - - - - - - - - - - - - - - - - - - -

cd ..\src

set "cpp=.\*.cc .\util\*.cc .\parse\*.cc .\object\*.cc .\vm\*.cc .\exec\*.cc"

g++ %cpp% -std=c++11 -static -o ..\build\def.exe

cd ..\build

Rem pause


