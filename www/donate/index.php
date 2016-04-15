<?php include "../header.php"; ?>

<script>$('#nav_donate').addClass('active');</script>

<link rel="stylesheet" type="text/css" href="/css/donate.css" />


<div id="donate" class="wrapaper">

<br>

<p>Def 遵循 MIT许可证，你可以自由的使用、修改、分享和发布，或将其集成用于商业用途。如果您在使用 Def 的过程中获益，或愿意支持 Def 的开发工作，我们接受您的捐助，用于新功能开发、BUG修改、库管理程序支持或服务器购买等。</p>

<br>
<p class="strong">您的每一笔捐款都将公布在捐助者名单里，并通过短信或邮件的方式告知您款项的具体用途和使用细节。</p>


<br>
<p>可通过支付宝或微信捐助：<p>

<br><br>
<p class="donate">
    <img src="/img/2wm/donate1.png" />　　　　　　　　　　　　
    <img src="/img/2wm/donate2.png" />
<p>
<br><br><br>

<p>当前您捐助的款项将被用于以下用途：<p>
<br>
<ul class="purpose">
    <li>清晰友好的错误提示：是用于准确定位错误的位置和类型，大幅度提升开发与调试效率</li>
    <li>跨平台支持：让 Def 可在 Linux 及 OS X 等诸多系统平台上使用</li>
    <li>新特征开发：例如 Lambda 表达式等高级抽象</li>
</ul>

<br>
<p>如果您对 Def 的技术实现感兴趣，请上 <a href="https://github.com/jojoin/Def" target="_blank">Github</a> 提交新的代码，一旦被采用，你的名字将被加入 Def 贡献者名单 :-D 。</p>



<br><br>
<h4 class="ptitle">捐助者名单</h4>


<?php $rosters = array(
    "*凯健,  6.66,  2016.04.11,  加油",
    "张昭程,  15.00,  2016.04.02,  支持",
    "郝俊东,  20.00,  2016.03.19,  错误提示太渣",
    "陈函,  9.90,  2016.03.11,  希望尽快加上 Lambda",
    "*应宸,  11.11,  2016.02.17,  无",
    "闫权,  5.00,  2016.02.03,  为理想主义",
); ?>


<br>
<table id="roster" cellspacing="0" border="1">
    <thead><tr>
        <td>姓名</td>
        <td>金额(￥)</td>
        <td>日期</td>
        <td>寄语</td>
    </tr></thead>
    <tbody>
    <?php foreach ($rosters as $v) {
        $one = explode(',',preg_replace("/[\s]{2,}/","",$v));
        echo "<tr><td>$one[0]</td><td>$one[1]</td><td>$one[2]</td><td>$one[3]</td></tr>";
    } ?>
    </tbody>
    
</table>




</div>


<?php include "../footer.php"; ?>


