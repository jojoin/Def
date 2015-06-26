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

# import "item/func.d" # 函数

import "item/proc.d" # 处理器

arr : (100 3.69 "yangjie")

print type(arr)

# 遍历容器
# proc.foreach{ arr k v
# 	print v
# }

