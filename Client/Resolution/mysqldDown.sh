#!/bin/bash

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

# modify the value of configuration option
let value/=2

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

# clear temp file
rm -fr error.txt tmp.txt
