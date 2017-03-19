#include <linux/module.h>
#include <linux/init.h>
#include "common/dateOper.h"
#include "log/logOper.h"
#include "common/fileOper.h"
#include "common/dirOper.h"
#include "common/strOper.h"
#include "running/resource.h"


int Code_init(void)
{
	char *path = "/proc/2398";
	
	char info[8][MAX_INFOLENGTH];
	int i;
	for(i = 0; i < 8; i++)
		memset(info[i], 0, MAX_INFOLENGTH);
	int ret = getProgressInfo(path, info);
	if(ret == -1)
		printk("getProgressInfo ret = %d\n", ret);
	for(i = 0; i < 8; i++)
	{
		printk("%10s\t", info[i]);
	}
	printk("\n");
	
	printk("success\n");
	
    return 0;
}

void Code_exit(void)
{
	printk("Done!\n");
    return;
}

module_init(Code_init);  
module_exit(Code_exit);  
