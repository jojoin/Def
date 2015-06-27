#--
	处理器调用测试
--#



# 定义处理器



# 遍历容器
def test{con}
	arr : evaluat(con) # 执行取值
	print( arr )
;



# 遍历容器
def foreach{con k v}
	arr : evaluat(con) # 执行取值
	ii : size(arr)
	i : 1
	while i<=ii
		assign(k i)
		assign(v arr[i])
		j : 4
		jj : size(_argv_)
		while j<=jj
			evaluat(_argv_[j]) # 执行循环体
			j : j+1
		;
		i : i+1
	;
;



