#!/bin/sh

echo "- - - - - - - - - - - - - - - - - - - - - - -"
echo "- Compile def vm c++ source code to def ..."
echo "- - - - - - - - - - - - - - - - - - - - - - -"

cd ../src

g++ ./*.cc ./util/*.cc ./parse/*.cc ./object/*.cc ./vm/*.cc ./exec/*.cc -std=c++11 -w -o ../build/def 

cd ../build

#echo -e "\nsuccessful ！"

