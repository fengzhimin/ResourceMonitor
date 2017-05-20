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
#include "running/resource.h"
#include "running/conflictCheck.h"
#include "messagePassing/kernel2user.h"
#include "resource/network/netResource.h"
#include "resource/device/DevResource.h"

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
			sprintf(ioUsedInfo, "%s %s:%d", ioUsedInfo, diskUsed->diskName, diskUsed->ioUsed);
			vfree(diskUsed);
		}
		while(totalResource.netUsed != NULL)
		{
			netUsed = totalResource.netUsed;
			totalResource.netUsed = totalResource.netUsed->next;
			sprintf(netUsedInfo, "%s %s:上传:%lld 下载:%lld", netUsedInfo, netUsed->netCardName, netUsed->downloadBytes, netUsed->uploadBytes);
			vfree(netUsed);
		}
		int i;
		printk("总CPU使用率为: %d\t总内存使用率为: %d\t IO使用率: %s\t NET使用率: %s\n", totalResource.cpuUsed, totalResource.memUsed, ioUsedInfo, netUsedInfo);
		solveProcessRelate(info, ret);

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
			if((totalResource.cpuUsed > 70 && info[i].cpuUsed > 30) || (totalResource.memUsed > 70 && info[i].memUsed > 30) || info[i].ioSyscallNum > 1000 || \
					(info[i].totalBytes > 2000000))
			{
				int conflictType = 0;
				if(totalResource.cpuUsed > 70 && info[i].cpuUsed > 30)
					conflictType |= CPU_CONFLICT;
				if(totalResource.memUsed > 70 && info[i].memUsed > 30)
					conflictType |= MEM_CONFLICT;
				if(info[i].ioSyscallNum > 1000)
					conflictType |= IO_CONFLICT;
				if(info[i].totalBytes > 2000000)
					conflictType |= NET_CONFLICT;

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

		vfree(info);
	}

	return 0;
}
