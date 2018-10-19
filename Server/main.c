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

int monitorResource(void *data)
{
	int i;
	while(!kthread_should_stop())
	{
		int avgCPU, avgMEM, avgSWAP;
		unsigned long long avgIOData, avgNetData;
		unsigned long avgMaj_flt;
		//当系统资源紧缺的时候才可能引发软件资源竞争情况发生
		if(judgeSysResConflict())
		{
			if(judgeSoftWareConflict())
			{
				printk("--------------------------start----------------------\n");
				bool IOConflict = false;
				bool NetConflict = false;
				currentDiskUsedInfo = beginDiskUsedInfo;
				for(i = 0; i < currentDiskNum; i++)
				{
					if(currentDiskUsedInfo->ioUsed >= SYS_MAX_IO)
						IOConflict = true;
					currentDiskUsedInfo = currentDiskUsedInfo->next;
				}
				currentNetUsedInfo = beginNetUsedInfo;
				for(i = 0; i < currentNetNum; i++)
				{
					if(currentNetUsedInfo->netUsed >= SYS_MAX_NET)
						NetConflict = true;
					currentNetUsedInfo = currentNetUsedInfo->next;
				}
				//系统资源冲突
				//加锁
				mutex_lock(&ConflictProcess_Mutex);

				//删除冲突信息
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
					avgCPU = avgMEM = avgSWAP = 0;
					avgIOData = avgNetData = avgMaj_flt = 0;
					
					for(i = 0; i < MAX_RECORD_LENGTH; i++)
					{
						avgCPU += currentMonitorAPP->cpuUsed[i];
						avgMEM += currentMonitorAPP->memUsed[i];
						avgSWAP += currentMonitorAPP->swapUsed[i];
						avgMaj_flt += currentMonitorAPP->maj_flt[i];
						avgIOData += currentMonitorAPP->ioDataBytes[i];
						avgNetData += currentMonitorAPP->netTotalBytes[i];
					}
					
					avgCPU /= MAX_RECORD_LENGTH;
					avgMEM /= MAX_RECORD_LENGTH;
					avgSWAP /= MAX_RECORD_LENGTH;
					avgMaj_flt /= MAX_RECORD_LENGTH;
					avgIOData /= MAX_RECORD_LENGTH;
					avgNetData /= MAX_RECORD_LENGTH;

					//冲突时进程资源使用情况
					ProcResUtilization conflictProcResUsed;

					conflictProcResUsed.cpuUsed = avgCPU;
				    conflictProcResUsed.memUsed = avgMEM;
					conflictProcResUsed.swapUsed = avgSWAP;
					conflictProcResUsed.ioDataBytes = avgIOData;
					conflictProcResUsed.netTotalBytes = avgNetData;

					//printk("%20s: %8d\t%8d\t%d\t%ld\t%8lld\t%8lld [%d\t%d]\n", currentMonitorAPP->name, avgCPU, avgMEM, avgSWAP, avgMaj_flt, avgIOData, avgNetData, aveWait_sum, aveIOWait_sum);
					char conflictType = 0;
					bool conflictPoint = false;
					if(avgCPU > PROC_MAX_CPU && avgSYSCpuUsed >= SYS_MAX_CPU)
					{
						printk("CPU conflict\n");
						conflictType |= CPU_CONFLICT;
						conflictPoint = true;
					}
					if((avgMEM+avgSWAP) > PROC_MAX_MEM && avgSYSMemUsed >= SYS_MAX_MEM && avgMaj_flt > PROC_MAX_MAJ_FLT)
					{
						printk("MEM conflict\n");
						conflictType |= MEM_CONFLICT;
						conflictPoint = true;
					}
					if(avgIOData > PROC_MAX_IO && IOConflict)
					{
						printk("IO conflict\n");
						conflictType |= IO_CONFLICT;
						conflictPoint = true;
					}
					if(avgNetData > PROC_MAX_NET && NetConflict)
					{
						printk("NET conflict\n");
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
							beginConflictProcess->pgid = currentMonitorAPP->pgid;
							beginConflictProcess->normalResUsed = currentMonitorAPP->normalResUsed;
							beginConflictProcess->conflictResUsed = conflictProcResUsed;
							beginConflictProcess->next = NULL;
						}
						else
						{
							endConflictProcess = endConflictProcess->next = vmalloc(sizeof(ConflictProcInfo));
							strcpy(endConflictProcess->name, currentMonitorAPP->name);
							endConflictProcess->conflictType = conflictType;
							endConflictProcess->pgid = currentMonitorAPP->pgid;
							endConflictProcess->normalResUsed = currentMonitorAPP->normalResUsed;
							endConflictProcess->conflictResUsed = conflictProcResUsed;
							endConflictProcess->next = NULL;
						}
					}
					currentMonitorAPP = currentMonitorAPP->next;
				}

				//释放锁
				mutex_unlock(&ConflictProcess_Mutex);
				printk("--------------------------end----------------------\n\n\n\n");
			}
			else
			{
				//删除冲突信息
				currentConflictProcess = beginConflictProcess;
				while(beginConflictProcess != NULL)
				{
					beginConflictProcess = beginConflictProcess->next;
					vfree(currentConflictProcess);
					currentConflictProcess = beginConflictProcess;
				}
			}
		}
	}

	return 0;
}
