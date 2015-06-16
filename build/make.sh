#!/bin/sh

echo -e "- - - - - - - - - - - - - - - - - - - - - - -"
echo -e "- Compile def vm c++ source code to def ..."
echo -e "- - - - - - - - - - - - - - - - - - - - - - -"

cd ../src

cc0=./util/path.cc 

cc1=./parse/node.cc ./parse/nodezer.cc ./parse/token.cc ./parse/tokenizer.cc

cc2=./object/object.cc ./object/operat.cc

cc3=./vm/exec.cc ./vm/gc.cc ./vm/stack.cc ./vm/module.cc

g++ ./def.cc $cc0 $cc1 $cc2 $cc3  -std=c++11 -w -o ../build/def 

cd ../build

#echo -e "\nsuccessful ！"

