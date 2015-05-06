#!/bin/sh

echo -e "Compile def vm c++ source code to def ..."

g++ ./def.cc ./vm/vm.cc ./vm/gc.cc ./vm/stack.cc ./module/error.cc ./object/object.cc ./object/operat.cc ./parse/node.cc ./parse/nodezer.cc ./parse/token.cc ./parse/tokenizer.cc -std=c++11 -w -o def 


echo -e "\nsuccessful ！"

