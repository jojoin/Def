<?php include "header.php"; ?>

<link rel="stylesheet" type="text/css" href="/css/index.css" />


<a id="forkme" target="_blank" href="https://github.com/jojoin/Def">
    <img src="/img/forkme.png">
</a>


<div id="banner"><div class="wrapaper">

    <div class="intro">
        <p><b>可扩展的编程语言</b> Def 致力于提供：</p>
        <div class="motto">类、泛型等不损失效率的抽象</div>
        <div class="motto">重载、宏等可扩展的元编程</div>
        <div class="motto">符号绑定等自定义语法</div>
        <div class="motto">缩减代码长度提升编码和编译效率</div>
        <p>通过编译强类型、完全类型推导、函数模板、嵌套函数、函数重载、类模板、扩展宏、模式匹配、 lambda表达式、闭包、操作符绑定等等一系列特征完成 Def 的目标：</p>
        <p><b>将 C++ 的高效抽象和 Lisp 的强大表现力融为一体</b>。</p>
    </div>

    <div id="codesample"><div class="n0"><code><span>;;; 声明 Point 类型</span>
<b>type</b> Point <i>(</i>    
    Int <v>x</v>    <span>;类成员</span>
    Int <v>y</v>
    <b>fun</b> Point<i>(</i>Int <u>a</u><i>)(</i>    <span>;构造函数</span>
        <b>set</b> <v>x</v> <u>a</u>    <span>;类成员赋值</span>
        <b>set</b> <v>y</v> <u>a</u>
    <i>)</i>
    <b>fun</b> gain<i>(</i>Int <u>a</u><i>)(</i>    <span>;成员函数</span>
        add <u>a</u> add <v>x y</v>    <span>;add 为加法函数</span>
    <i>)</i>
<i>)</i>
<b>var</b> pt Point <n>1</n>    <span>;构造 Point 并赋值给变量 pt</span></code></div><div class="n1"><code><span>;;; 操作符绑定</span>
<b>var</b> n <n>1</n>    <span>;定义变量 n 类型为 Int</span>
<b>set</b> n add n <n>2</n> <span>;与 2 相加并赋值: n = n + 2</span>
<b>let</b><i>(</i> <u>a</u><b>++</b> <i>)(</i> <b>set</b> <u>a</u> add <u>a</u> <n>1</n> <i>)</i> <span>;绑定 ++ 操作符</span>
<b>let</b><i>(</i> <u>a</u><b>+=</b><u>b</u> <i>)(</i> <b>set</b> <u>a</u> add <u>a</u> <u>b</u> <i>)</i> <span>;绑定 += 操作符</span>
n <b>++</b> <span>;等同于: n = n + 1</span>
n <b>+=</b> <n>7</n> <span>;等同于: n = n + 7</span>
<span>;;; 自定义语法：元组解包</span>
<b>let</b> unpack<i>(</i><u>names val</u><i>)(</i>
    <b>mcrfor</b><i>(</i><u>names</u><i>)(</i> <b>var</b> <v>_v</v> <b>elmget</b> <u>val</u> <v>_i</v> <i>)</i>
<i>)</i>
<b>var</b> vt tuple<i>(</i> <n>1 2 3</n> <i>)</i>    <span>; 定义元组变量</span>
unpack <i>(</i><u>a b c</u><i>)</i> vt    <span>; 解包元组并赋值 a=1,b=2,c=3</span>
</code></div><div class="n2"><code><span>;;; 函数模板</span>
<b>tpf</b> algor<i>(</i><u>x y</u><i>)(</i>    <span>;自动推导参数与返回值类型</span>
    div add <u>x y</u> sub <u>x y</u>    <span>;等同于 (x+y)/(x-y)</span>
<i>)</i>
<b>var</b> iv algor <n>1 2</n>    <span>;实例化整形参数 algor 函数</span>
<b>var</b> fv algor <n>1.5 2.9</n>    <span>;实例化浮点形参数函数</span>
<span>;;; 类模板</span>
<b>tpty</b> TClass <i>(</i>T1 T2<i>) (</i>
    T1 <v>vtx</v>
    <b>array</b> <n>5</n> T2 <v>vtary</v>    <span>;长度为5的数组类成员</span>
    <b>fun</b> TClass<i>(</i>T1 <u>t</u><i>)(</i>    <span>;构造函数</span>
        <b>set</b> <v>vtx</v> <u>t</u>  <i>)</i>
    <b>fun</b> getIdx2<i>(</i><i>)(</i>    <span>;获取数组成员</span>
        <b>arrget</b> <v>vtary</v> <n>2</n>  <i>) )</i>    <span>;vtary[2]</span>
</code></div></div>
 <div class="clear"></div>
</div></div>

<script type="text/javascript">

(function(){

var $code = $("#codesample")
  , $codeone = $code.children("div")
  , len = $codeone.length
  , i = 0
  ;

setInterval(function(){
    i++;
    if(i==len){
        i = 0;
    }
    $codeone.slideUp();
    $codeone.filter('.n'+i).slideDown();
}, 6000);


})();

$('#nav_index').addClass('active');

</script>




<div class="wrapaper"><br><br>

<!--微博-->
<div id="weibo">
    <iframe width="100%" height="416" class="share_self"  frameborder="0" scrolling="no" src="http://widget.weibo.com/weiboshow/index.php?language=&width=0&height=416&fansRow=2&ptype=1&speed=0&skin=5&isTitle=1&noborder=1&isWeibo=1&isFans=1&uid=5888762623&verifier=e5c5ac2e&dpc=1"></iframe>
</div>

<div id="content">
    <p>hello world ：</p> 
    <p><code>puts <u>"hello world !"</u>  <span>;打印</span></code></p>
    <p>或许你已经发现，Def 的 hello world 程序与 C++ 或 Java 等传统的编译强类型语言不一样，反而更像是 Python、Ruby 或 JavaScript 等脚本。没错，Def 的目标就是让用户体验如同编写脚本一样简单轻松，同时拥有接近目标机器代码的高性能。</p>
    <p>不同于 <code class="line"><i>(</i><u>a</u> <i>+</i> <u>b</u><i>) * (</i>x <i>-</i> y<i>)</i></code> 等常见C风格语法，Def 采用类似 <code class="line">mul add <u>a b</u> sub <u>x y</u></code> 的 “auto-expression” 表达式，统一函数调用、宏调用、模板实例化等语法。</p>
    <p>如果要了解 Def 语言全部的特征，请查看 <a href="https://github.com/jojoin/Def/tree/master/test/script" target="_blank">标准测试代码</a> 示例。</p>
</div>

<div class="clear"></div>
<br>

<!--友言评论-->
<script type="text/javascript">
var uyan_config = {
     'du':'http://www.deflang.org', 
};
</script>
<div id="uyan_frame"></div>
<script type="text/javascript" src="http://v2.uyan.cc/code/uyan.js?uid=2043454"></script>
</div>

<?php include "footer.php"; ?>


