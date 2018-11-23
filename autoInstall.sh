#!/bin/bash

#######################################################
# Author       : fengzhimin
# Create       : 2018-09-20 04:24
# Last modified: 2018-09-20 04:24
# Email        : 374648064@qq.com
# Filename     : autoInstall.sh
# Description  : auto-install Server
#######################################################

if [ $# != 1 ]
then
	echo "please input the path of linux-3.16.40.tar.gz"
	echo "Example: sudo ./autoInstall.sh /home/linux-3.16.40.tar.gz"
	exit -1
fi

if [ ! -e $1 ]
then
	echo -e "\033[31m $1 is not exist! \033[0m"
	exit -1
fi

user=`whoami`
if [ $user != "root" ]
then
	echo "please use root to execute!"
	exit -1
fi

if [ -d /usr/src/linux-3.16.40 ]
then
	rm -fr /usr/src/linux-3.16.40
fi

# tar linux-3.16.40.tar.gz
echo "begin to decompress $1 ..."
tar -zxvf $1 -C /usr/src > /dev/null

if [ $? != 0 ]
then
	echo -e "\033[31m decompress $1 failure! \033[0m"
	exit -1
else
	echo "decompress $1 success!"
fi

kernelPath=/usr/src/linux-3.16.40

#copy .config
cp /usr/src/linux-headers*-generic/.config $kernelPath

if [ $? != 0 ]
then
	echo -e "\033[31m copy .config failure! \033[0m"
	exit -1
fi

# copy new file
cp ./Server/kernel-3-16-40/af_inet.c ${kernelPath}/net/ipv4
if [ $? != 0 ]
then
	echo -e "\033[31m copy af_inet.c failure! \033[0m"
	exit -1
else
	echo "copy af_inet.c success!"
fi

cp ./Server/kernel-3-16-40/monitorResource.h ${kernelPath}/include/linux
if [ $? != 0 ]
then
	echo -e "\033[31m copy monitorResource.h failure! \033[0m"
	exit -1
else
	echo "copy monitorResource.h success!"
fi
	
cp ./Server/kernel-3-16-40/monitorResource.c ${kernelPath}/fs
if [ $? != 0 ]
then
	echo -e "\033[31m copy monitorResource.c failure! \033[0m"
	exit -1
else
	echo "copy monitorResource.c success!"
fi

cp ./Server/kernel-3-16-40/Makefile ${kernelPath}/fs
if [ $? != 0 ]
then
	echo -e "\033[31m copy Makefile failure! \033[0m"
	exit -1
else
	echo "copy Makefile success!"
fi

# modify source code
vfs_readdir=${kernelPath}/include/linux/fs.h
if [ -e $vfs_readdir ]
then
	sed -i '2546d' $vfs_readdir
	echo "modify vfs_readdir success!"
else
	echo -e "\033[31m $vfs_readdir is not existing! \033[0m"
	exit -1
fi

get_cmdline=${kernelPath}/mm/util.c
if [ -e $get_cmdline ]
then
	sed -i '504i EXPORT_SYMBOL(get_cmdline);' $get_cmdline
	echo "modify get_cmdline success!"
else
	echo -e "\033[31m $get_cmdline is not existing! \033[0m"
	exit -1
fi

next_zone=${kernelPath}/mm/mmzone.c
if [ -e $next_zone ]
then
	sed -i '44i EXPORT_SYMBOL(next_zone);' $next_zone
	echo "modify next_zone success!"
else
	echo -e "\033[31m $next_zone is not existing! \033[0m"
	exit -1
fi

first_online_pgdat=${kernelPath}/mm/mmzone.c
if [ -e $first_online_pgdat ]
then
	sed -i '16i EXPORT_SYMBOL(first_online_pgdat);' $first_online_pgdat
	echo "modify first_online_pgdat success!"
else
	echo -e "\033[31m $first_online_pgdat is not existing! \033[0m"
	exit -1
fi

si_swapinfo=${kernelPath}/mm/swapfile.c
if [ -e $si_swapinfo ]
then
	sed -i '2583i EXPORT_SYMBOL(si_swapinfo);' $si_swapinfo
	echo "modify si_swapinfo success!"
else
	echo -e "\033[31m $si_swapinfo is not existing! \033[0m"
	exit -1
fi

nsecs_to_jiffies64=${kernelPath}/kernel/time.c
if [ -e $nsecs_to_jiffies64 ]
then
	sed -i '689i EXPORT_SYMBOL(nsecs_to_jiffies64);' $nsecs_to_jiffies64
	echo "modify nsecs_to_jiffies64 success!"
else
	echo -e "\033[31m $nsecs_to_jiffies64 is not existing! \033[0m"
	exit -1
fi

__lock_task_sighand=${kernelPath}/kernel/signal.c
if [ -e $__lock_task_sighand ]
then
	sed -i '1287i EXPORT_SYMBOL_GPL(__lock_task_sighand);' $__lock_task_sighand
	echo "modify __lock_task_sighand success!"
else
	echo -e "\033[31m $__lock_task_sighand is not existing! \033[0m"
	exit -1
fi

thread_group_cputime_adjusted=${kernelPath}/kernel/sched/cputime.c
if [ -e $thread_group_cputime_adjusted ]
then
	sed -i '630i EXPORT_SYMBOL_GPL(thread_group_cputime_adjusted);' $thread_group_cputime_adjusted
	sed -i '452i EXPORT_SYMBOL_GPL(thread_group_cputime_adjusted);' $thread_group_cputime_adjusted
	echo "modify thread_group_cputime_adjusted success!"
else
	echo -e "\033[31m $thread_group_cputime_adjusted is not existing! \033[0m"
	exit -1
fi

# start to compile kernel and install
cd $kernelPath
make -j8 && make modules_install && make install

# clear previous installation
if [ -e /boot/vmlinuz-3.16.40.old ]
then
	rm -fr /boot/vmlinuz-3.16.40.old
	rm -fr /boot/Systemp.map-3.16.40.old
	rm -fr /boot/initrd.img-3.16.40.old
fi

if [ $? == 0 ]
then
	echo "compile kernel(version: 3.16.40) success!"
	read -p "Do you want to reboot? <Y/N>" isReboot
	case $isReboot in
	Y | y)
		reboot;;
	*)
		exit 0;;
	esac
fi
