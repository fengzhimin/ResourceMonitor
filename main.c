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
#include "messagePassing/kernel2user.h"
#include "resource/network/netResource.h"
#include "resource/device/DevResource.h"

static struct task_struct *monitorTask = NULL;
static DEFINE_MUTEX(mymutex);
static int max_CPUUSE = 70, max_MEMUSE = 70;

/**********************************
 * func: 创建内核线程用于监控资源使用情况
 * return: void
 * @para data: 线程参数
**********************************/
int monitorResource(void *data);

int Code_init(void)
{
	printk("success\n");
	//从配置文件中提取CPU和MEM的最大临界值
	char max_CPUUSE_Str[CONFIG_VALUE_MAX_NUM];
	char *max_CPUUSE_ptr = max_CPUUSE_Str;
	char max_MEMUSE_Str[CONFIG_VALUE_MAX_NUM];
	char *max_MEMUSE_ptr = max_MEMUSE_Str;
	memset(max_CPUUSE_Str, 0, CONFIG_VALUE_MAX_NUM);
	memset(max_MEMUSE_Str, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByKey("CPU", &max_CPUUSE_ptr))
	{
		max_CPUUSE = ExtractNumFromStr(max_CPUUSE_ptr);
	}
	else
	{
		printk("提取配置项CPU的值失败!\n");
	}

	if(getConfValueByKey("MEM", &max_MEMUSE_ptr))
	{
		max_MEMUSE = ExtractNumFromStr(max_MEMUSE_ptr);
	}
	else
	{
		printk("提取配置项MEM的值失败!\n");
	}

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
			sprintf(ioUsedInfo, "%s %s:%d%", ioUsedInfo, diskUsed->diskName, diskUsed->ioUsed);
			vfree(diskUsed);
		}
		while(totalResource.netUsed != NULL)
		{
			netUsed = totalResource.netUsed;
			int speed = getNetCardSpeed(netUsed->netCardName);
			totalResource.netUsed = totalResource.netUsed->next;
			//计算出来的是百分比
			if(speed != 0)
				sprintf(netUsedInfo, "%s %s:%d%", netUsedInfo, netUsed->netCardName, netUsed->totalBytes/(speed*10000));
			else
				sprintf(netUsedInfo, "%s %s:%d%", netUsedInfo, netUsed->netCardName, 0);
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
				//if(info[i].cpuUsed > 30 || info[i].memUsed > 30 || info[i].ioSyscallNum > 1000 || info[i].totalBytes > 2000000)
				{
					int conflictType = 0;
					if(info[i].cpuUsed > 30)
						conflictType |= CPU_CONFLICT;
					if(info[i].memUsed > 30)
						conflictType |= MEM_CONFLICT;
					/*
					if(info[i].ioSyscallNum > 1000)
						conflictType |= IO_CONFLICT;
					if(info[i].totalBytes > 2000000)
						conflictType |= NET_CONFLICT;
					*/
					//printk("conflictType = %d\n", conflictType);
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
					//printk("进程 %s: PID: %d PPID: %d CPU使用率: %d MEM使用率: %d  IO次数: %lld 读写磁盘数据: %lld,  upload: %lld  download: %lld  total: %lld\n", \
						info[i].name, info[i].pid, info[i].ppid,	info[i].cpuUsed, info[i].memUsed, info[i].ioSyscallNum, \
						info[i].ioDataBytes, info[i].uploadBytes, info[i].downloadBytes, info[i].totalBytes);
				}
			}
			//处理端口冲突的问题
			/*
			currentConflictPortProcInfo = beginConflictPortProcInfo;
			if(currentConflictPortProcInfo != NULL)
			{
				if(beginConflictProcess == NULL)
				{
					beginConflictProcess = endConflictProcess = currentConflictProcess = vmalloc(sizeof(ConflictProcInfo));
					beginConflictProcess->next = NULL;
				}
				else
				{
					endConflictProcess = endConflictProcess->next = vmalloc(sizeof(ConflictProcInfo));
					endConflictProcess->next = NULL;
				}
				endConflictProcess->conflictType |= PORT_CONFLICT;
				sprintf(endConflictProcess->conflictInfo, "进程:%s(%d) 与 进程:%s(%d) 在使用端口号为: %d 上冲突!\n", \
				currentConflictPortProcInfo->currentProcess.ProcessName, currentConflictPortProcInfo->currentProcess.pid, \
				currentConflictPortProcInfo->runningProcess.ProcessName, currentConflictPortProcInfo->runningProcess.pid, \
				currentConflictPortProcInfo->port);
				beginConflictPortProcInfo = beginConflictPortProcInfo->next;
				vfree(currentConflictPortProcInfo);
				currentConflictPortProcInfo = beginConflictPortProcInfo;
			}
			*/
			//释放锁
			mutex_unlock(&ConflictProcess_Mutex);
		}

		vfree(info);
	}

	return 0;
}
