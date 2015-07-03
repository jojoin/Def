
@echo off
echo - - - - - - - - - - - - - - - - - - - - - -
echo - Compile def vm c++ source code to def.exe
echo - - - - - - - - - - - - - - - - - - - - - -

cd ..\src

:: 合并文件 exec.cc
set "target=.\vm\exec.cc"
type .\exec\_header.c > %target%
for %%f in (.\exec\*.cc) do type %%f >> %target%
type .\exec\_footer.c >> %target%

:: cpp文件目录
set "cpp=.\*.cc .\util\*.cc .\parse\*.cc .\object\*.cc .\vm\*.cc"

:: 开始编译 -static
g++ %cpp% -std=c++11 -w -o ..\build\def.exe

cd ..\build

Rem pause


