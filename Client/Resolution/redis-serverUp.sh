#!/bin/bash
# $1 : the name of configuration option
# $2 : the default value of configuration option

maxCount=5

# get the value of configuration option
redis-cli config get $1 > tmp.txt 2> error.txt
while [ $? -ne 0 ]
do
	if [ $maxCount -eq 0 ]; then
		rm -fr tmp.txt error.txt
		exit 1
	fi
	let maxCount-=1
	redis-cli config get $1 > tmp.txt 2> error.txt
done

value=`cat tmp.txt | tail -n 1 | cut -d "\"" -f 1`

if [ $value -ge $2 ]; then
	rm -fr tmp.txt error.txt
	exit 1
fi

# modify the value of configuration option
let value+=1048576    # 1M

if [ $value -ge $2 ]; then
	value=$2
fi

maxCount=5

# set the value of configuration option
redis-cli config set $1 ${value} > /dev/null 2> error.txt
while [ $? -ne 0 ]
do
	if [ $maxCount -eq 0 ]; then
		rm -fr tmp.txt error.txt
		exit 1
	fi
	let maxCount-=1
	redis-cli config set $1 ${value} > /dev/null 2> error.txt
done

# clear temp file
rm -fr error.txt tmp.txt
