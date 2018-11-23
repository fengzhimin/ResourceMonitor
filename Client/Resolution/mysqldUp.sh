#!/bin/bash
# $1 : the name of configuration option
# $2 : the default value of configuration option

maxCount=5

# get the value of configuration option
mysql -uroot -pfzm --execute="show variables like '$1'" > tmp.txt 2> error.txt
while [ $? -ne 0 ]
do
	if [ $maxCount -eq 0 ]; then
		exit 1
	fi
	let maxCount-=1
	mysql -uroot -pfzm --execute="show variables like '$1'" > tmp.txt 2> error.txt
done

value=`cat tmp.txt | tail -n 1 | awk '{print $2}'`

if [ $value -ge $2 ]; then
	exit 1
fi

# modify the value of configuration option
let value+=10    # 1M

if [ $value -ge $2 ]; then
	value=$2
fi

maxCount=5

# set the value of configuration option
mysql -uroot -pfzm --execute="set global $1=${value}" > /dev/null 2> error.txt
while [ $? -ne 0 ]
do
	if [ $maxCount -eq 0 ]; then
		exit 1
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
		exit 1
	fi
	let maxCount-=1
	mysql -uroot -pfzm --execute="flush tables" > /dev/null 2> error.txt
done
