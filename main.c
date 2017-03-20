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

static char stat_data[1000];
static char error_info[200];
static char subStr18[18][MAX_SUBSTR];
static struct task_struct *monitorTask = NULL;

/**********************************
 * func: 创建内核线程用于监控资源使用情况
 * return: void
 * @para data: 线程参数
**********************************/
int monitorResource(void *data);

int Code_init(void)
{
	/*
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
	*/
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
	char info[8][MAX_INFOLENGTH];
	char path[30];
	while(!kthread_should_stop())
	{
		task = &init_task;
		list_for_each(ps, &task->tasks)
		{
			p = list_entry(ps, struct task_struct, tasks);
			count++;
			//printk("%d\t%s\n", p->pid, p->comm);
			
			if(strcmp(p->comm, "monitorKthread") == 0)
			{
				memset(stat_data, 0, 1000);
				memset(path, 0, 30);
				sprintf(path, "%s/%d/stat", "/proc", p->pid);
				//if(KReadLine(fp, stat_data) == -1)
				struct file* fp = KOpenFile(path, O_RDONLY);
				int size = KReadFile(fp, stat_data, 1000);
				if(size > 0)
				{
					printk("size = %d\n", size);
					cutStrByLabel(stat_data, ' ', subStr18, 18);
					printk("pid = %d\n", ExtractNumFromStr(subStr18[12]));
					printk("utime = %d\n", ExtractNumFromStr(subStr18[13]));
					printk("stime = %d\n", ExtractNumFromStr(subStr18[14]));
					printk("cutime = %d\n", ExtractNumFromStr(subStr18[15]));
					printk("cstime = %d\n", ExtractNumFromStr(subStr18[16]));
					KCloseFile(fp);
				}
				else
				{
					sprintf(error_info, "%s%s%s%s%s", "读取文件: ", path, " 失败！ 错误信息： ", "    ", "\n");
					printk("error = %s\n", error_info);
					KCloseFile(fp);
				}
				/*
				int i;
				memset(path, 0, 30);
				sprintf(path, "%s/%d", "/proc", p->pid);
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
				*/
			}
		}
		//printk("Process counts: %d\n", count);
		//printk("data = %s\n", (char *)data);
		msleep(500);
	}

	return 0;
}
