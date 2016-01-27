#!/bin/sh

echo "- - - - - - - - - - - - - - - - - - - - - - -"
echo "- Compile def vm c++ source code to def ..."
echo "- - - - - - - - - - - - - - - - - - - - - - -"

cd ../src

# 合并文件 exec.cc
target=./vm/exec.cpp
cat ./exec/_header.c > $target
for file in ./exec/*.cpp
do 
	cat $file >> $target 
done
cat ./exec/_footer.c >> $target

# 开始编译 -static
g++ ./*.cpp ./util/*.cpp ./parse/*.cpp ./object/*.cpp ./vm/*.cpp -std=c++11 -w -o ../build/def 

cd ../build

#echo -e "\nsuccessful ！"

