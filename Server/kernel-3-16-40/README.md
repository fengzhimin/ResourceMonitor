### 拷贝文件af_inet.c: /net/ipv4
- sudo cp af_inet.c /usr/src/linux-3.16.40/net/ipv4

### 拷贝文件monitorResource.h: /include/linux
- sudo cp monitorResource.h /usr/src/linux-3.16.40/include/linux

### 拷贝文件monitorResource.c: fs/
- sudo cp monitorResource.c /usr/src/linux-3.16.40/fs

### 拷贝文件Makefile: fs/
- sudo cp Makefile /usr/src/linux-3.16.40/fs

### 注释掉源码中的/usr/src/linux-3.16.40/include/linux/fs.h中的vfs_readdir声明

### export get_cmdline function(/usr/src/linux-3.16.40/mm/util.c)
- 在函数定义结尾加EXPORT_SYMBOL(get_cmdline);

### export next_zone function(/usr/src/linux-3.16.40/mm/mmzone.c)
- 在函数定义结尾加EXPORT_SYMBOL(next_zone);

### export first_online_pgdat(/usr/src/linux-3.16.40/mm/mmzone.c)
- 在函数定义结尾加EXPORT_SYMBOL(first_online_pgdat);

### export si_swapinfo(/usr/src/linux-3.16.40/mm/swapfile.c)
- 在函数定义结尾加EXPORT_SYMBOL(si_swapinfo);

### export nsecs_to_jiffies64(/usr/src/linux-3.16.40/kernel/time.c)
- 在函数定义结尾加EXPORT_SYMBOL(nsecs_to_jiffies64);

### export __lock_task_sighand(/usr/src/linux-3.16.40/kernel/signal.c) EXPORT_SYMBOL_GPL
- 在函数定义结尾加EXPORT_SYMBOL_GPL(__lock_task_sighand);

### export thread_group_cputime_adjusted(/usr/src/linux-3.16.40/kernel/sched/cputime.c)  EXPORT_SYMBOL_GPL
- 在函数定义结尾加EXPORT_SYMBOL_GPL(thread_group_cputime_adjusted);  注意：源文件中有两处定义
