Def
===

### 可扩展的编程语言

![Def Logo](http://www.deflang.org/image/logo_small.png)

### 简介

Def 致力于提供：

1. 类、泛型等不损失效率的抽象
2. 重载、宏等可扩展的元编程
3. 符号绑定等自定义语法
4. 缩减代码长度提升编码和编译效率

通过编译强类型、完全类型推导、函数模板、嵌套函数、函数重载、类模板、扩展宏、模式匹配、 lambda表达式、闭包、操作符绑定等等一系列特征完成 Def 的目标：**将 C++ 的高效抽象和 Lisp 的元编程融为一体**。

### 链接

[官网](http://deflang.org/)　　[教程](http://deflang.org/manual/)　　[文档](http://deflang.org/document/)　　[关于](http://deflang.org/about.php)

### 代码示例

```INI
;; 自定义类型
type Point (
    Int x
    Int y
)
;; 定义变量并初始化
var p1 Point 1 2   ; 类型初始化语法与函数调用相同
var p2 Point 3 4
;; 重载 add 函数
fun add(Point a Point b)(
    ret Point     ;  缩进和换行不是必须，仅为了排版美观
        add 
            elmget a x  ; elmget 表示类成员访问
            elmget b x
        add 
            elmget a y
            elmget b y
)
;; 重载函数调用
add p1 p2

;; 添加操作符绑定
let ( a + b ) ( add a b )
let ( a := b ) ( var a b )
let ( a.b ) ( elmget a b )
;; 自定义类型
type Point (
    Int x
    Int y
)
;; 定义变量并初始化
p1 := Point 1 2   ; 类型初始化语法与函数调用相同
p2 := Point 3 4
;; 重载 add 函数，利用函数模板
tpf add(a b)(  ; tpf 定义函数模板
    Point (a.x)+(a.y) (b.x)+(b.y)
)
;; 重载的模板函数调用
p1 + p2

;; Def 多返回值函数
fun getTuple()(
    ret tuple (  ; 返回元组类型
        10
        9.9
        'a'
    )
)
;; 得到元组并使用
var tp getTuple  ; 是的，函数调用不需要括号
var num elmget tp 0  ; num = tp[0] 

;; 定义元组拆包宏 
let upk(names val)(  ;  upk 为 unpack 的缩写
    mcrfor(names)( var _v elmget val _i )
    ; mcrfor 表示宏参数循环  _v 为值  _i 为循环索引
)

;; 使用拆包宏
upk(a b c) tp
```

### 许可

MIT
