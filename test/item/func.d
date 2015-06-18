#--
	函数调用测试
--#



p : 123

# 函数定义（默认参数和关键字参数）
# 
defun test(a b c:p d:"ddd")
	nnn : "cond 'a' is true, return !!! "
	if a
		return nnn
	;
	"$a is false , d: "+d
;


# 空函数调用
defun ept()
;

