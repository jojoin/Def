Def
===

![Def Logo](http://www.deflang.org/image/logo_small.png)


###【新版本开发中，编译强类型，支持完全类型推断、函数模板、函数重载、模式匹配、嵌套闭包、操作符绑定等高级功能，敬请期待...】

.

.

.

.

以下为过期内容...

.

.

.

.

.

.


Def 是新一代基于虚拟机的开源通用编程语言，拥有 C 的简捷语法和 Lisp 的强大表现力。

Def 支持多范式、反射、元编程、函数式、垃圾回收等高级特征。通过强大的独一无二的处理器语法，让 Def 成为“可编程的编程语言”，甚至在运行时期改变程序行为。运用其精简直观的语法，可快速构建出模块化、易重构、可读性强的应用程序。

　

### >>> 链接

　[官方网站](http://www.deflang.org/)　　\　　[快速开始](http://www.deflang.org/getstart)　　\　　[文档手册](http://www.deflang.org/document)　　\　　[下载](http://www.deflang.org/download)

　

### >>> 使用

下载 Def 虚拟机（可执行文件）：

　　Windows : [def-win64.exe](http://www.deflang.org/download/def-win64.exe)  
　　Linux :  [def-linux64](http://www.deflang.org/download/def-linux64)  
　　OS X : [def-osx64](http://www.deflang.org/download/def-osx64)  

你也可以[下载源码](https://codeload.github.com/yangjiePro/Def/zip/master)自行编译得到 Def 虚拟机。通过执行 `build`文件夹下的编译脚本，通过支持 C++11 标准的编译器编译。 

打开命令行，切换带虚拟机可执行文件所在目录，执行 `def -c` 便可输入 Def 脚本，回车运行并得到结果。或者输入 `def hello.d` 执行 Def 脚本文件。 

　

### >>> 代码示例

```python
# 变量定义
num : 100
word : "hello " + "world !"

# 列表
list : (1 2 3 4 5)

# 字典
dict : [
  'name'   "def lang"
  'number' (1 2 3)
]

# 容器访问
list[3] # 3
dict['name'] # def lang

# 函数定义
fn add(a b)
  a + b
;

# 函数调用
add(1 2) # 3

# 处理器定义（遍历容器）
def foreach{con k v}
	arr : evaluat(con) # 执行取值
	ii  : size(arr)
	i   : 1
	while i<=ii
		assign(k i)
		assign(v arr[i])
		j  : 4
		jj : size(_argv_)
		while j<=jj
			evaluat(_argv_[j]) # 执行循环体
			j : j+1
		;
		i : i+1
	;
;

# 处理器调用
con : ("def" 2015 12.5)
foreach{ con k v
  print(v) # 打印每一项
}
```

　

### >>> 其它

本项目采用 MIT 协议。

作者：杨捷  
邮箱：yangjie@jojoin.com
