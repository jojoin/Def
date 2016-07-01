<?php include "../header.php"; ?>

<script>$('#nav_download').addClass('active');</script>

<link rel="stylesheet" type="text/css" href="/css/download.css" />

<div id="ptitle"><p><i>下载</i></p></div>


<div id="download" class="wrapaper">

<h5>源码</h5>

<p>Def 的源码及相关文档均托管在 Github ，<a target="_blank" href="https://github.com/jojoin/Def">点这里</a> 查看，也可以下载源码：</p>

<a class="downzip" target="_blank" href="https://github.com/jojoin/Def/archive/master.zip">下载 v0.1.7 (zip)</a>

<h5>依赖</h5>

<ul>
    <li><a target="_blank" href="http://www.cplusplus.com/reference/clibrary/">C Standard Library</a></li>
    <li><a target="_blank" href="http://www.cplusplus.com/">C++ 11</a></li>
    <li><a target="_blank" href="http://llvm.org/">LLVM 3.8.0</a></li>
</ul>


<h5>预编译文件</h5>

<p>每一个版本发布，我们将提供主流平台的预编译版本。如果在以下的列表中未找到您对应的系统环境，请 <a target="_blank" href="https://github.com/jojoin/Def/archive/master.zip">下载源码</a> 并通过 <a target="_blank" href="https://github.com/jojoin/Def/blob/master/test/def.bat">build脚本</a> 在您的机器上编译。<p>

<ul>
    <li><a target="_blank" href="/build/Windows-x64-Release-v0.1.7.exe">
        Windows-x64-Release-v0.1.7.exe
    </a></li>
    <li><a target="_blank" href="/build/Windows-x86-Release-v0.1.7.exe">
        Windows-x86-Release-v0.1.7.exe
    </a></li>
    <li><a target="_blank" href="/build/Windows-x64-Debug-v0.1.7.exe">
        Windows-x64-Debug-v0.1.7.exe
    </a></li>
    <li><a target="_blank" href="/build/Windows-x86-Debug-v0.1.7.exe">
        Windows-x86-Debug-v0.1.7.exe
    </a></li>
</ul>


<h5>命令行使用</h5>

<p>在下载或编译得到 Def 编译器的可执行文件后，通过命令行参数即可编译、调试、打印相关信息。可用参数如下：</p>

<p><code>  <b>-h, --help</b>     显示帮助
  <b>--version</b>      显示版本信息
  <v>[File]</v>         要编译的 def 语言文件
  <b>-d</b> <v>String</v>, <b>--debug</b> <v>String</v>   打印调试信息
  <b>-p</b> <v>String</v>, <b>--print</b> <v>String</v>   打印 相关信息
  <b>-e</b> <v>String</v>, <b>--emit</b> <v>String</v>    编译生成相关结果
  <b>-o</b> <v>String</v>, <b>--output</b> <v>String</v>  编译结果输出文件
</code></p>

<p>如果要了解详细的编译参数，请参看 <a target="_blank" href="https://github.com/jojoin/Def/blob/master/test/def.bat">编译脚本示例</a> 。</p>



</div>

















<?php include "../footer.php"; ?>


