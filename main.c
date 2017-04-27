#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include "common/dateOper.h"
#include "log/logOper.h"
#include "common/fileOper.h"
#include "common/dirOper.h"
#include "common/strOper.h"
#include "running/resource.h"
#include "running/conflictCheck.h"
#include "resource/device/DevResource.h"

char info[10][MAX_INFOLENGTH];
static struct task_struct *monitorTask = NULL;

/**********************************
 * func: 创建内核线程用于监控资源使用情况
 * return: void
 * @para data: 线程参数
**********************************/
int monitorResource(void *data);

int Code_init(void)
{
	printk("success\n");

	monitorTask = kthread_create(monitorResource, "hello kernel thread", "monitorKthread");
	if(IS_ERR(monitorTask))
	{
		printk("Unable to start kernel thread\n");
		int err = PTR_ERR(monitorTask);
		monitorTask = NULL;
		return err;
	}
	wake_up_process(monitorTask);

	return 0;
}

void Code_exit(void)
{
	printk("Done!\n");
	if(monitorTask)
	{
		printk("Cancel this kernel thread\n");
		kthread_stop(monitorTask);
		printk("Canceled\n");
	}
    return;
}

module_init(Code_init);  
module_exit(Code_exit);

int monitorResource(void *data)
{
	while(!kthread_should_stop())
	{
		char ***info;
		char totalResource[2][MAX_INFOLENGTH];
		int ret = getProgressInfo(&info, totalResource);
		int i;
		printk("总CPU使用率为: %s\t总内存使用率为: %s\n", totalResource[0], totalResource[1]);
		for(i = ret/2; i < ret; i++)
		{
			printk("进程 %s: PID: %s PPID: %s CPU使用率: %s MEM使用率: %s  IO次数: %s\n", info[i][0], info[i][1], info[i][2], \
					info[i][3], info[i][4], info[i][8]);
		}

		printk("解决进程之间冲突问题\n");
		printk("解决进程之间冲突问题\n");
		printk("解决进程之间冲突问题\n");
		solveProcessRelate(info, ret);	
		for(i = ret/2; i < ret; i++)
		{
			printk("进程 %s: PID: %s PPID: %s CPU使用率: %s MEM使用率: %s IO次数: %s\n", info[i][0], info[i][1], info[i][2], \
					info[i][3], info[i][4], info[i][8]);
		}
		freeResource(info, ret, PROCESS_INFO_NUM);
		msleep(10000);
	}

	return 0;
}
