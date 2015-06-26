#--
	函数调用测试
--#









p : (1 2 3)

# 向上查找赋值
defun addP(v)
	p[] : v
;

# 向上查找赋值
defun setP(k v)
	p[k] : v
;

defun getP()
	p
;


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


# 回调函数
defun callback(func p1 p2:'' p3:'')
	func(p1 p2 p3)
;




