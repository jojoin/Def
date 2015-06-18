#!/bin/sh

echo "- - - - - - - - - - - - - - - - - - - - - - -"
echo "- Compile def vm c++ source code to def ..."
echo "- - - - - - - - - - - - - - - - - - - - - - -"

cd ../src

g++ ./def.cc      ./util/path.cc      ./parse/node.cc ./parse/nodezer.cc ./parse/token.cc ./parse/tokenizer.cc       ./object/object.cc ./object/operat.cc     ./vm/exec.cc ./vm/gc.cc ./vm/stack.cc ./vm/module.cc        -std=c++11 -w -o ../build/def 

cd ../build

#echo -e "\nsuccessful ！"

