#!/bin/bash
# $1 : the name of configuration option
# 返回值:
# 0 = 增加配置项成功
# 2 = 获取配置项值失败
# 3 = 更新配置项值失败
# 4 = 其他操作失败

maxCount=5

# get the value of configuration option
redis-cli config get $1 > tmp.txt 2> error.txt
while [ $? -ne 0 ]
do
	if [ $maxCount -eq 0 ]; then
		exit 2
	fi
	let maxCount-=1
	redis-cli config get $1 > tmp.txt 2> error.txt
done

value=`cat tmp.txt | tail -n 1 | cut -d "\"" -f 1`

# modify the value of configuration option
let value/2

maxCount=5

# set the value of configuration option
redis-cli config set $1 ${value} > /dev/null 2> error.txt
while [ $? -ne 0 ]
do
	if [ $maxCount -eq 0 ]; then
		exit 3
	fi
	let maxCount-=1
	redis-cli config set $1 ${value} > /dev/null 2> error.txt
done

exit 0
