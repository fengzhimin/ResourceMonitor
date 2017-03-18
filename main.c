#include <linux/module.h>
#include <linux/init.h>
#include "common/dateOper.h"
#include "log/logOper.h"
#include "common/fileOper.h"
#include "common/dirOper.h"

int Code_init(void)
{
	int ret = Is_Dir("/etc");
	if(ret == -1)
		printk("是文件夹\n");
	else if(ret == -2)
		printk("是文件\n");
/*
	struct file *fd = KOpenFile("/etc", O_RDONLY);
	struct dir_context dir;
	iterate_dir(fd, &dir);
	printk("data = %s\n", dir);
*/
    return 0;
}

void Code_exit(void)
{
	printk("Done!\n");
    return;
}

module_init(Code_init);  
module_exit(Code_exit);  
