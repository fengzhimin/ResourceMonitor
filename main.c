#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/monitorResource.h>
#include "common/dateOper.h"
#include "log/logOper.h"
#include "common/fileOper.h"
#include "common/dirOper.h"
#include "common/strOper.h"
#include "common/confOper.h"
#include "running/resource.h"
#include "running/conflictCheck.h"
#include "running/monitorSoftWare.h"
#include "running/process.h"
#include "messagePassing/kernel2user.h"

static struct task_struct *monitorTask = NULL;
static DEFINE_MUTEX(mymutex);

MODULE_LICENSE("Dual BSD/GPL");

/**********************************
 * func: 创建内核线程用于监控资源使用情况
 * return: void
 * @para data: 线程参数
**********************************/
int monitorResource(void *data);

int Code_init(void)
{
	printk("success\n");

#if (MONITOR_TYPE == 0)
	getMonitorSoftWare();
#endif

	loadConfig();   //read config information from configuration file
	memset(sysResArray, 0, sizeof(SysResource)*MAX_RECORD_LENGTH);    //clear sysResArray
	monitorTask = kthread_create(monitorResource, "hello kernel thread", "monitorKthread");
	if(IS_ERR(monitorTask))
	{
		printk("Unable to start kernel thread\n");
		int err = PTR_ERR(monitorTask);
		monitorTask = NULL;
		return err;
	}
	mutex_init(&ConflictProcess_Mutex);
	init_Netlink();
	wake_up_process(monitorTask);

	return 0;
}

void Code_exit(void)
{
	printk("Done!\n");
	if(monitorTask)
	{
		release_Netlink();
		printk("Cancel this kernel thread\n");
		kthread_stop(monitorTask);
		printk("Canceled\n");
	}
    return;
}

module_init(Code_init);  
module_exit(Code_exit);

/***print history resource unilization
int monitorResource(void *data)
{
	int i;
	while(!kthread_should_stop())
	{
		getSysResourceInfo();
		if(judgeSysResConflict())
		{
			getUserLayerAPP();
			currentMonitorAPP = beginMonitorAPP;
			while(currentMonitorAPP != NULL)
			{
				printk("%20s:", currentMonitorAPP->name);
				for(i = 0; i < MAX_RECORD_LENGTH; i++)
					printk("[%3d %3d]\t", currentMonitorAPP->cpuUsed[i], currentMonitorAPP->memUsed[i]);
				printk("\n");
				currentMonitorAPP = currentMonitorAPP->next;
			}
		}
	}

	return 0;
}
*/


int monitorResource(void *data)
{
	int i;
	while(!kthread_should_stop())
	{
		int avgCPU, avgMEM;
		unsigned long long avgIOData, avgNetData;
	//	getSysResourceInfo();
		//if(judgeSysResConflict())
		{
			if(judgeSoftWareConflict())
			{
		printk("--------------------------start----------------------\n");
				//系统资源冲突
				//加锁
				mutex_lock(&ConflictProcess_Mutex);

				currentConflictProcess = beginConflictProcess;
				while(beginConflictProcess != NULL)
				{
					beginConflictProcess = beginConflictProcess->next;
					vfree(currentConflictProcess);
					currentConflictProcess = beginConflictProcess;
				}

				currentMonitorAPP = beginMonitorAPP;
				while(currentMonitorAPP != NULL)
				{
					avgCPU = avgMEM = 0;
					avgIOData = avgNetData = 0;
					for(i = 0; i < MAX_RECORD_LENGTH; i++)
					{
						avgCPU += currentMonitorAPP->cpuUsed[i];
						avgMEM += currentMonitorAPP->memUsed[i];
						avgIOData += currentMonitorAPP->ioDataBytes[i];
						avgNetData += currentMonitorAPP->netTotalBytes[i];
					}
					avgCPU /= MAX_RECORD_LENGTH;
					avgMEM /= MAX_RECORD_LENGTH;
					avgIOData /= MAX_RECORD_LENGTH;
					avgNetData /= MAX_RECORD_LENGTH;
					printk("%20s: %8d\t%8d\t%8lld\t%8lld\n", currentMonitorAPP->name, avgCPU, avgMEM, avgIOData, avgNetData);
					int conflictType = 0;
					bool conflictPoint = false;
					if(avgCPU > PROC_MAX_CPU)
					{
						conflictType |= CPU_CONFLICT;
						conflictPoint = true;
					}
					if(avgMEM > PROC_MAX_MEM)
					{
						conflictType |= MEM_CONFLICT;
						conflictPoint = true;
					}
					if(avgIOData > PROC_MAX_IO)
					{
						conflictType |= IO_CONFLICT;
						conflictPoint = true;
					}
					if(avgNetData > PROC_MAX_NET)
					{
						conflictType |= NET_CONFLICT;
						conflictPoint = true;
					}
					if(conflictPoint)
					{
						if(beginConflictProcess == NULL)
						{
							beginConflictProcess = endConflictProcess = currentConflictProcess = vmalloc(sizeof(ConflictProcInfo));
							strcpy(beginConflictProcess->name, currentMonitorAPP->name);
							beginConflictProcess->conflictType = conflictType;
							beginConflictProcess->next = NULL;
						}
						else
						{
							endConflictProcess = endConflictProcess->next = vmalloc(sizeof(ConflictProcInfo));
							strcpy(endConflictProcess->name, currentMonitorAPP->name);
							endConflictProcess->conflictType = conflictType;
							endConflictProcess->next = NULL;
						}
					}
					currentMonitorAPP = currentMonitorAPP->next;
				}

				//释放锁
				mutex_unlock(&ConflictProcess_Mutex);
			}
		}
		printk("--------------------------end----------------------\n\n\n\n");
	}

	return 0;
}
