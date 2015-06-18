#--

 Def 语言测试执行文件

 测试内容：列表 或 字典 的访问和赋值

--#



# print "---------  test container  ----------"


# 列表（索引从1开始）

li : ( 1 2 3 "yang" "jie"  ('a' 'b') )

# print li[4]      # 访问
# print li[6][1]   # 多层访问

li[1] : 111           # 替换
li[]  : "the last"    # 末尾追加

# print li

# 字典

key : "vk",
val : "variable value",
dt : [
	'k1'   123
	key    "variable key"
	'val'  val
]


# print dt['k1']    # 访问
dt['123'] : 123

dt[key] : "the new variable key"  # 重新赋值

dt['vbb'] : 'variable1adsfagff'

# print dt




# 复合结构

obj : [
	'name'   "yangjie"
	'age'    24
	'mynum'  (1 2 3 2014 2015 2016)
	'user'  [
		'yangjie'  24
		'liyang'   21
		'other'    0
	]
]

obj['user']['other'] : 99  # 多层访问赋值

# print obj


# print "---------  test end  ----------"


