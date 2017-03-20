#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
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
	struct task_struct *task, *p;
	struct list_head *ps;
	int count = 0;
	char path[30];
	while(!kthread_should_stop())
	{
		task = &init_task;
		list_for_each(ps, &task->tasks)
		{
			p = list_entry(ps, struct task_struct, tasks);
			count++;
			//printk("%d\t%s\n", p->pid, p->comm);
			
			if(p->pid > 1000)
			{
				int i;
				memset(path, 0, 30);
				sprintf(path, "%s/%d", "/proc", p->pid);
				for(i = 0; i < 10; i++)
					memset(info[i], 0, MAX_INFOLENGTH);
				int ret = getProgressInfo(path, info);
				if(ret == -1)
					printk("getProgressInfo ret = %d\n", ret);
			/*	
				for(i = 0; i < 8; i++)
				{
					switch(i)
					{
					case 0:
						printk("0" "%30s", info[i]);
						break;
					case 1:
						printk("0" "%10s", info[i]);
						break;
					case 2:
						printk("0" "%10s", info[i]);
					break;
					case 3:
						printk("0" "%5s", info[i]);
						break;
					case 4:
						printk("0" "%5s", info[i]);
						break;
					case 5:
						printk("0" "%15s", info[i]);
						break;
					case 6:
						printk("0" "%15s", info[i]);
						break;
					case 7:
						printk("0" "%10s", info[i]);
						break;
					}
				}
				*/
				printk("CPU: %s\t MEM: %s", info[8], info[9]);
				printk("\n");
			}
		}
		//printk("Process counts: %d\n", count);
		//printk("data = %s\n", (char *)data);
		msleep(500);
	}

	return 0;
}
