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
		getSysResourceInfo();
		if(judgeSysResConflict())
		{
			if(judgeSoftWareConflict())
			{
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
						avgIOData += currentMonitorAPP->ioDataBytes;
						avgNetData += currentMonitorAPP->netTotalBytes;
					}
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
					if(avgCPU > PROC_MAX_IO)
					{
						conflictType |= IO_CONFLICT;
						conflictPoint = true;
					}
					if(avgCPU > PROC_MAX_NET)
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
	}

	return 0;
}


/* 同时获取系统的资源以及每个进程的资源使用
int monitorResource(void *data)
{
	while(!kthread_should_stop())
	{
		
		ProcInfo *info;
		SysResource totalResource;
		int ret = getProgressInfo(&info, &totalResource);
		//合并磁盘数据
		char ioUsedInfo[100] = { 0 };
		char netUsedInfo[100] = { 0 };
		IOUsedInfo *diskUsed = NULL;
		NetUsedInfo *netUsed = NULL;
		while(totalResource.ioUsed != NULL)
		{
			diskUsed = totalResource.ioUsed;
			totalResource.ioUsed = totalResource.ioUsed->next;
			sprintf(ioUsedInfo, "%s %s:%d", ioUsedInfo, diskUsed->diskName, diskUsed->ioUsed);
			vfree(diskUsed);
		}
		while(totalResource.netUsed != NULL)
		{
			netUsed = totalResource.netUsed;
			totalResource.netUsed = totalResource.netUsed->next;
			//跳过lo网卡，因为在获取lo的带宽时会发生错误，导致内存不断的泄漏
			if(strcasecmp(netUsed->netCardName, "lo") != 0)
			{
				int speed = getNetCardSpeed(netUsed->netCardName);
				//计算出来的是百分比
				if(speed != 0)
					sprintf(netUsedInfo, "%s %s:%d", netUsedInfo, netUsed->netCardName, netUsed->totalBytes/(speed*10000));
				else
					sprintf(netUsedInfo, "%s %s:%d", netUsedInfo, netUsed->netCardName, 0);
			}
			vfree(netUsed);
		}
		int i;
		printk("总CPU使用率为: %d\t总内存使用率为: %d\t IO使用率: %s\t NET使用率: %s\n", totalResource.cpuUsed, totalResource.memUsed, ioUsedInfo, netUsedInfo);
		solveProcessRelate(info, ret);

		if(totalResource.cpuUsed > max_CPUUSE || totalResource.memUsed > max_MEMUSE)
		{
			//加锁
			mutex_lock(&ConflictProcess_Mutex);

			currentConflictProcess = beginConflictProcess;
			while(beginConflictProcess != NULL)
			{
				beginConflictProcess = beginConflictProcess->next;
				vfree(currentConflictProcess);
				currentConflictProcess = beginConflictProcess;
			}

			for(i = 0; i < ret; i++)
			{
				if(strcasecmp(info[i].name, "monitorKthread") == 0)
					continue;
				//判断是那个进程占用资源多
				if(info[i].cpuUsed > 30 || info[i].memUsed > 30 || info[i].ioSyscallNum > 1000 || info[i].totalBytes > 2000000)
				{
					int conflictType = 0;
					if(info[i].cpuUsed > 30)
						conflictType |= CPU_CONFLICT;
					if(info[i].memUsed > 30)
						conflictType |= MEM_CONFLICT;

					if(beginConflictProcess == NULL)
					{
						beginConflictProcess = endConflictProcess = currentConflictProcess = vmalloc(sizeof(ConflictProcInfo));
						beginConflictProcess->processInfo = info[i];
						beginConflictProcess->conflictType = conflictType;
						beginConflictProcess->next = NULL;
					}
					else
					{
						endConflictProcess = endConflictProcess->next = vmalloc(sizeof(ConflictProcInfo));
						endConflictProcess->processInfo = info[i];
						endConflictProcess->conflictType = conflictType;
						endConflictProcess->next = NULL;
					}
				}
			}
			//释放锁
			mutex_unlock(&ConflictProcess_Mutex);
		}

		vfree(info);
	}

	return 0;
}
*/
