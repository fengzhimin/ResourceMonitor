#!/bin/bash
# $1 : the name of configuration option
# $2 : increase the level of configuration option value
# $3 : the default value of configuration option
# 返回值:
# 0 = 增加配置项成功
# 1 = 自动增加到默认值
# 2 = 获取配置项值失败
# 3 = 更新配置项值失败
# 4 = 其他操作失败

maxCount=5

# get the value of configuration option
mysql -uroot -pfzm --execute="show variables like '$1'" > tmp.txt 2> error.txt
while [ $? -ne 0 ]
do
	if [ $maxCount -eq 0 ]; then
		exit 2
	fi
	let maxCount-=1
	mysql -uroot -pfzm --execute="show variables like '$1'" > tmp.txt 2> error.txt
done

value=`cat tmp.txt | tail -n 1 | awk '{print $2}'`

if [ $value -ge $3 ]; then
	exit 1
fi

# modify the value of configuration option
let value+=$2

if [ $value -ge $3 ]; then
	value=$3
fi

maxCount=5

# set the value of configuration option
mysql -uroot -pfzm --execute="set global $1=${value}" > /dev/null 2> error.txt
while [ $? -ne 0 ]
do
	if [ $maxCount -eq 0 ]; then
		exit 3
	fi
	let maxCount-=1
	mysql -uroot -pfzm --execute="set global $1=${value}" > /dev/null 2> error.txt
done

maxCount=5

# flush tables
mysql -uroot -pfzm --execute="flush tables" > /dev/null 2> error.txt
while [ $? -ne 0 ]
do
	if [ $maxCount -eq 0 ]; then
		exit 4
	fi
	let maxCount-=1
	mysql -uroot -pfzm --execute="flush tables" > /dev/null 2> error.txt
done

exit 0
