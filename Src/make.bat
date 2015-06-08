
@echo off
echo - - - - - - - - - - - - - - - - - - - - - -
echo - Compile def vm c++ source code to def.exe
echo - - - - - - - - - - - - - - - - - - - - - -

g++ .\def.cc .\vm\vm.cc .\vm\gc.cc .\vm\stack.cc .\module\error.cc .\object\object.cc .\object\operat.cc .\parse\node.cc .\parse\nodezer.cc .\parse\token.cc .\parse\tokenizer.cc -std=c++11  -o .\def.exe

Rem pause


