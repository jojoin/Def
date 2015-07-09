#--
	处理器调用测试
--#



# 自定义的 switch case 结构
def myswitch{}
	base : none
	hd : true
	getcase : false
	k : 0
	len : size(_argv_)
    while k < len
    	k : k + 1
    	v : _argv_[k]
    	if 1=k
    		base : evaluat( v ) # 判断条件
    		continue
    	;
    	# printl(v.type())
    	isbreak : 'variable'=v.type() & 'end'=v.name()
    	if getcase
	    	if isbreak
	    		break
	    	;
	    	evaluat( v ) # 执行case体
	    	continue
    	;
    	if hd
    		cond : evaluat( v ) # case 条件
    		if base=cond # case 条件为真
    			getcase : true
    		;
    		hd : false
    	;
    	if isbreak
    		hd : true
    	;
    ;
;


#--
# 调用自定义的 switch 语句
k : 'b'
myswitch{ k
	'a'
		printl('k is "a"')
	end
	'b'
		printl('k is "b"')
	end
}
--#


# 处理器定义（遍历数组）
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




#--
# 自定义的遍历器调用
con : ("def" 2015 12.5)
foreach{ con k v
  printl(v) # 打印每一项
}
--#
