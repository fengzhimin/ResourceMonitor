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

#include "resource/network/netResource.h"

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
		
		ProcInfo *info;
		SysResource totalResource;
		int ret = getProgressInfo(&info, &totalResource);
		int i;
		//printk("总CPU使用率为: %d\t总内存使用率为: %d\t 上传速度: %lld\t 下载速度:%lld\n", totalResource.cpuUsed, totalResource.memUsed, \
				totalResource.uploadBytes, totalResource.downloadBytes);
		solveProcessRelate(info, ret);	
		for(i = 0; i < ret; i++)
		{
			if(strcasecmp(info[i].name, "monitorKthread") == 0)
				continue;
			if((totalResource.cpuUsed > 70 && info[i].cpuUsed > 30) || (totalResource.memUsed > 70 && info[i].memUsed > 30) || info[i].ioSyscallNum > 1000 || \
					(info[i].totalBytes > 2000000 && totalResource.totalBytes > 6000000))
				printk("进程 %s: PID: %d PPID: %d CPU使用率: %d MEM使用率: %d  IO次数: %lld 读写磁盘数据: %lld,  upload: %lld  download: %lld  total: %lld\n", \
					info[i].name, info[i].pid, info[i].ppid,	info[i].cpuUsed, info[i].memUsed, info[i].ioSyscallNum, \
					info[i].ioDataBytes, info[i].uploadBytes, info[i].downloadBytes, info[i].totalBytes);
		}
		vfree(info);
	}

	return 0;
}
