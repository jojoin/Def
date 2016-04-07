<?php include "../header.php"; ?>

<link rel="stylesheet" type="text/css" href="/css/manual.css" />

<div id="ptitle"><p><i>入门使用教程</i></p></div>


<div id="manual" class="wrapaper">

<h4>目录</h4>
<p class="menu"><b></b>
    <a href="#syntax">语法</a>
    <a href="#overload">函数重载</a>
    <a href="#macro">宏</a>
</p>


<a name="syntax"></a>

<h4>语法</h4>

<p>了解一门语言的首要就是了解它的语法。我们将通过几个简单的与C系列语法对比的例子，快速形成对 Def 语法的直观印象。</p>

<p>函数调用：</p>

<code>foo<i>(</i><u>a</u><i>,</i> <u>b</u><i>,</i> <n>10</n><i>);</i> <em>; C++</em>
foo <u>a b</u> <n>10</n>    <em>; Def 参数直接跟在函数名称后面，不加任何修饰</em></code>

<p>宏展开：</p>

<code>DEFBAR<i>(</i><u>X</u><i>,</i> <u>Y</u><i>)</i>  <em>; C++</em>
DEFBAR <u>X Y</u>    <em>; Def 宏参直接给出，不加修饰</em></code>

<p>类模板：</p>

<code>TplClass<i>&lt;</i><v>Point</v><i>,</i> <v>Int</v><i>&gt;</i>  tcla<i>;</i>    <em>; C++</em>
TplClass <v>Point Int</v> tcla        <em>; Def 类型参数如同函数参数一样，同样不加修饰</em></code>

<p>通过上面的代码示例，我们可以从中发现规律：Def 的语法特点，就是 “<b>不加任何修饰</b>” 。可能你仍然感到疑惑：函数调用、宏展开及类模板的语法在C++里有通过不同的符号拥有不同的表达方式，但 Def 这三者却看不出任何区别，都是直接简单的将所需的内容排列出来，这样的话，Def 岂不是根本没有语法？</p>

<p>没错，Def 的语法就是没有语法！</p>

<p>这似乎打破了大家学习一门语言的基本套路：1. 先熟悉语法；2. 进行简单的运算；</p>

<p>如果 Def 没有语法，那将如何表示加减乘除这些四则运算？我们来看：</p>

<code><em>; C 系列语法四则运算</em>
<i>(</i> <n>1</n> <i>+</i> <n>2</n> <i>)</i> <i>*</i> <i>(</i> <n>3</n> <i>-</i> <n>4</n> <i>)</i>   <em>; -3</em>
<em>; Def 语法</em>
mul add <n>1 2</n> sub <n>3 4</n>    <em>; -3</em>
</code>

<p>在 Def 中，<cd>1+2</cd> 表示为 <cd>add 1 2</cd>，与上文的函数调用一样。其实， <cd>1+2</cd> 这种被称作表达式的东西本质上仍然是一个函数，只是使用的频率非常高，通常语言的设计者为了省去书写麻烦而特意添加的语法糖。</p>

<p>如果你想问，Def 的使用者有没有这种糖吃？答案是：取决于你自己。Def 将制造糖的权力交给用户，每个人都可以做出符合自己口味的糖，而不用受语言设计者的约束。为了方便理解，我们先来为四则运算加糖，修改上文的例子：</p>

<code><em>; Def 语法 表示 (1+2)*(3-4)</em>
mul add <n>1 2</n> sub <n>3 4</n>    <em>; -3</em>
<em>; 加糖：操作符绑定</em>
<b>let</b> <i>(</i> <u>a</u> <i>+</i> <u>b</u> <i>) (</i> add <u>a b</u> <i>)</i>
<b>let</b> <i>(</i> <u>a</u> <i>-</i> <u>b</u> <i>) (</i> sub <u>a b</u> <i>)</i>
<b>let</b> <i>(</i> <u>a</u> <i>*</i> <u>b</u> <i>) (</i> mul <u>a b</u> <i>)</i>
<em>; 吃糖 ！</em>
<i>(</i> <n>1</n> <i>+</i> <n>2</n> <i>) * (</i> <n>3</n> <i>-</i> <n>4</n> <i>)</i>   <em>; -3</em>
</code>

<p>或许你仍然感到疑虑：为什么不预先把糖加好，这样难道不更方便？自定义操作符绑定除了给用户创造语法的能力，配合函数重载将极大的提升语言的表现力。我们将通过下文的例子解答你的疑虑。</p>

<a name="overload"></a>

<h4>函数重载</h4>

<p><cd>add 1 2</cd> 是 Def 内置的函数，表示两个 Int 类型相加，任何地方都可以使用。也可以通过函数重载的方式用于自定义类型：</p>

<code><em>;; 自定义类型</em>
<b>type</b> Point <i>(</i>
    Int <v>x</v>
    Int <v>y</v>
<i>)</i>
<em>;; 定义变量并初始化</em>
<b>var</b> p1 Point <n>1 2</n>   <em>; 类型初始化语法与函数调用相同</em>
<b>var</b> p2 Point <n>3 4</n>
<em>;; 重载 add 函数</em>
<b>fun</b> add<i>(</i>Point <u>a</u> Point <u>b</u><i>)(</i>
    <b>ret</b> Point     <em>;  缩进和换行不是必须，仅为了排版美观</em>
        add 
            <b>elmget</b> <u>a</u> x  <em>; elmget 表示类成员访问</em>
            <b>elmget</b> <u>b</u> x
        add 
            <b>elmget</b> <u>a</u> y
            <b>elmget</b> <u>b</u> y
<i>)</i>
<em>;; 重载函数调用</em>
add p1 p2</code>

<p>现在，我们用操作符绑定改写上面的代码：</p>

<code><em>;; 添加操作符绑定</em>
<b>let</b> <i>(</i> <u>a</u> <i>+</i> <u>b</u> <i>) (</i> add <u>a b</u> <i>)</i>
<b>let</b> <i>(</i> <u>a</u> <i>:=</i> <u>b</u> <i>) (</i> <b>var</b> <u>a b</u> <i>)</i>
<b>let</b> <i>(</i> <u>a</u><i>.</i><u>b</u> <i>) (</i> <b>elmget</b> <u>a b</u> <i>)</i>
<em>;; 自定义类型</em>
<b>type</b> Point <i>(</i>
    Int <v>x</v>
    Int <v>y</v>
<i>)</i>
<em>;; 定义变量并初始化</em>
p1 <i>:=</i> Point <n>1 2</n>   <em>; 类型初始化语法与函数调用相同</em>
p2 <i>:=</i> Point <n>3 4</n>
<em>;; 重载 add 函数，利用函数模板</em>
<b>tpf</b> add<i>(</i><u>a</u> <u>b</u><i>)(</i>  <em>; tpf 定义函数模板</em>
    Point <i>(</i><u>a</u><i>.</i><v>x</v><i>)+(</i><u>a</u><i>.</i><v>y</v><i>) (</i><u>b</u><i>.</i><v>x</v><i>)+(</i><u>b</u><i>.</i><v>y</v><i>)</i>
<i>)</i>
<em>;; 重载的模板函数调用</em>
p1 <i>+</i> p2</code>

<p>通过函数重载和操作符绑定，我们能轻松扩展语言的内核，按需提升语言的表现力，达到元编程的目的。</p>

<a name="macro"></a>

<h4>宏</h4>

<p>提到元编程，我们将更进一步，探讨一下在 Def 中自定义语法结构的表达能力。</p>

<p>考虑到函数的多返回值情况，某些现代的高级编程语言一般内置多返回值的语法，例如在 Go 语言中：</p>
<code><b>func</b> getTuple<i>(){</i>
    <b>var</b> x <i>int</i> <i>=</i> <n>9</n><i>;</i>
    <b>var</b> y <i>int</i> <i>=</i> <n>10</n><i>;</i>
    <b>return</b> x<i>,</i> y
<i>}</i>
<em>// 解包函数多返回值，语言内置语法</em>
x<i>,</i> y <i>=</i> getTuple<i>();</i>
</code>

<p>而在 Def 被没用内置这种语法。不过好消息是，通过强大的宏，你可以自定义创造这种语法：</p>

<code><em>;; Def 多返回值函数</em>
<b>fun</b> getTuple<i>()(</i>
    <b>ret</b> <i>tuple (</i>  <em>; 返回元组类型</em>
        <n>10
        9.9</n>
        <u>'a'</u>
    <i>)</i>
<i>)</i>
<em>;; 得到元组并使用</em>
<b>var</b> tp getTuple  <em>; 是的，函数调用不需要括号</em>
<b>var</b> num elmget tp <n>0</n>  <em>; num = tp[0] </em>

<em>;; 定义元组拆包宏 </em>
<b>let</b> upk<i>(</i><u>names val</u><i>)(</i>
    <b>mcrfor</b><i>(</i><u>names</u><i>)(</i> <b>var</b> <v>_v</v> <b>elmget</b> <u>val</u> <v>_i</v> <i>)</i>
    <em>; mcrfor 表示宏参数循环  _v 为值  _i 为循环索引</em>
<i>)</i>

<em>;; 使用拆包宏</em>
upk<i>(</i>a b c<i>)</i> tp
</code>

<p>定义 <cd>upk</cd> 为元组拆包宏，调用展开后的结果为：</p>

<code><b>var</b> a <b>elmget</b> tp <n>0</n>
<b>var</b> b <b>elmget</b> tp <n>1</n>
<b>var</b> c <b>elmget</b> tp <n>2</n>
<em>; a=10 , b=9.9 ,f c='a' </em>
</code>

<p>如果直接对函数返回值进行拆包：</p>

<code>upk<i>(</i>a b c<i>)</i> getTuple
</code>

<p>展开后的代码为：</p>

<code><b>var</b> a <b>elmget</b> getTuple <n>0</n>
<b>var</b> b <b>elmget</b> getTuple <n>1</n>
<b>var</b> c <b>elmget</b> getTuple <n>2</n>
</code>

<p>可以看见，<cd>getTuple</cd> 函数在拆包时被调用了三次，这并不是我们想要的结果。如果你想问，有没有办法让函数仅调用一次呢？我们尝试改写拆包宏：</p>

<code><b>let</b> upk<i>(</i><u>names val</u><i>)(</i>
    <b>var</b> upk_tmp val <em>; 定义中间变量，缓存函数调用结果</em>
    <b>mcrfor</b><i>(</i><u>names</u><i>)(</i> <b>var</b> <v>_v</v> <b>elmget</b> upk_tmp <v>_i</v> <i>)</i>
<i>)</i>
</code>

<p>新的拆包宏，展开后的代码为：</p>

<code><b>var</b> upk_tmp getTuple  <em>; 缓存函数返回值</em>
<b>var</b> a <b>elmget</b> upk_tmp <n>0</n>
<b>var</b> b <b>elmget</b> upk_tmp <n>1</n>
<b>var</b> c <b>elmget</b> upk_tmp <n>2</n>
</code>

<p>借助定义的中间变量 <cd>upk_tmp</cd> ，我们似乎已经解决了函数多次调用的问题。</p>

<p>通过 Def 强大的宏，我们创造了语言中原本不存在的语法！这在通常的编程语言中基本是不可想象的事情。你也许会等不及把刚才写的 <cd>upk</cd> 宏提取出来，放到公共的库里，以便在各处都能使用。真是激动人心！</p>

<p>但是一旦在同一个作用域内使用这个解包宏超过两次，你就会收到一个重复定义 <cd>upk_tmp</cd> 变量的错误消息，原因是我们在宏内部定义的变量并不是局部的，随着宏的展开，变量会在当前函数作用域内生效，这将导致重定义错误。</p>

<p>真是令人沮丧，如果我们的宏不能作为库使用，需要在每一个作用域内单独定义，而且只能使用一次，这真是太糟糕了，还不如不用。有没有办法让宏内部定义的变量是仅局部可见的？</p>

<p>如果让宏内部定义的变量仅局部可见，那么我们定义的 <cd>a,b,c</cd> 变量在宏调用结束后仍然无法使用，这是我们的拆包宏便失去了原本应有的功能。</p>

<p>现在的问题变成了：能不能让宏内部的变量，某一部分局部可见，另一部分向上层作用域开放？</p>












uvnnew abc ; 创建全局唯一名称

uvnget abc ; 获取全局唯一名称

uvndel abc ; 删除
uvnclear   ; 清空





</div>














<?php include "../footer.php"; ?>


