#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include "common/dateOper.h"
#include "log/logOper.h"
#include "common/fileOper.h"
#include "common/dirOper.h"
#include "common/strOper.h"
#include "running/resource.h"

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
		char totalResouce[2][MAX_INFOLENGTH];
		int ret = getProgressInfo(info, totalResouce);
		printk("ret = %d\n", ret);
		/*
		if(ret == -1)
			printk("getProgressInfo ret = %d\n", ret);
		else
		{
			int CPU = ExtractNumFromStr(info[8]);
			int MEM = ExtractNumFromStr(info[9]);
			//printk("CPU = %d\t MEM = %d\n", CPU, MEM);
			if(CPU > 80)
			{
				int pCPU = ExtractNumFromStr(info[3]);
				if(pCPU > 30)
					printk("进程 %s：占用CPU资源为:%d\n", info[0], pCPU);
			}
			if(MEM > 80)
			{
				int pMEM = ExtractNumFromStr(info[4]);
				if(pMEM > 30)
					printk("进程 %s: 占用MEM资源为:%d\n", info[0], pMEM);
			}
		}*/
		msleep(100);
	}

	return 0;
}
