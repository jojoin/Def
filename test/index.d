#--
 Def 语言测试执行文件
--#


# 模块导入测试

# 导入系统模块
# import "fs" 

# import "container.d" # 容器
# print container.li

# import "modules/math.d" # 运算
# print math.PI

import "item/func.d" # 运算

n : 2

# 函数调用
print func.test(
	n-1
	999
	"yangjie"
)



# # 空函数调用
# func.ept(1 2 3)

# 回调函数测试
print func.callback(
	func.test
	n-2
	999
	"yangjie"
)




