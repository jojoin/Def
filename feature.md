Def 语言设计原则
===

1. 面向底层和系统，编译为机器码
2. 零成本抽象（不使用的特性不支付成本）最小化运行时
3. 强类型，内存安全（生命周期）多线程安全，错误处理安全
4. 面向"类型"编程，函数类型和类类型都是一等公民
5. 全面的类型推导，现代范型系统，静态派发和动态派发自动识别，类型别名、类型扩展
6. 类型默认实现，范型偏特化，自动识别接口
7. 图灵完备的现代宏，元编程范式，语法糖开关
8. 变量、函数、方法、运算符重载
9. 用注释实现编译器标注、提示。意味着编译器可以无视、酌情处理这些标注
10. 标准库定制开关化（以适用嵌入式平台或高级操作系统编程）

代价：
1. 编译器实现复杂
2. 编译时间长（可通过部分编译、增量编译缓存优化）
3. 学习曲线陡峭

关键特性：
1. 自动类型推导：静态派发和动态派发，默认类型实现，默认函数实现
2. 已知尺寸类型，未知尺寸类型，自动识别和包装
3. 生命周期推断：编译期确定，运行期确定


---------------------------------

1. 内存空间
---

变量由储存空间及管理方式区分，分为三种：

 - 炉： 仅作用域， 创建关键字： var ， 局部变量，迭代变量，
 - 栈： 仅作用域， 创建关键字： let ， 局部定值、函数参数，【不可变】
 - 池： 垃圾回收， 创建关键字： allot ， 
 - 堆： 手动管理， 创建关键字： new ， 【不可变】
 - 铸： 全局存在， 创建关键字： const， 静态变量、常量、字符串、函数、编译器参数等，【不可变】



2. 结构化
---

struct
class
interface 
