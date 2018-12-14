/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-11 10:02
* Last modified: 2017-04-11 10:02
* Email        : 374648064@qq.com
* Filename     : conflictCheck.c
* Description  : 
******************************************************/

#include "running/conflictCheck.h"

bool judgeSysResConflict()
{
	//获取系统总的资源使用情况
	getSysResourceInfo();

	//一段时间内系统的资源平均使用情况
	int sumCPU = 0, sumMem = 0, sumSwap = 0;
	int i;
	for(i = 0; i < MAX_RECORD_LENGTH; i++)
	{
		sumCPU += sysResArray[i].cpuUsed;
		sumMem += sysResArray[i].memUsed;
		sumSwap += sysResArray[i].swapUsed;
	}
	avgSYSCpuUsed = sumCPU/MAX_RECORD_LENGTH;
	avgSYSMemUsed = sumMem/MAX_RECORD_LENGTH;
	avgSYSSwapUsed = sumSwap/MAX_RECORD_LENGTH;
	//judge whether CPU resource is conflicting
	if(avgSYSCpuUsed >= SYS_MAX_CPU)
	{
		CPUConflict = true;
#if (SHOWINFO == 1)
		printk("\033[33mCPU: %3d\t\033[0m", avgSYSCpuUsed);
#endif
	}
	else
	{
		CPUConflict = false;
#if (SHOWINFO == 1)
		printk("CPU: %3d\t", avgSYSCpuUsed);
#endif
	}
	//judge whether MEM resource is conflicting
	if(avgSYSMemUsed >= SYS_MAX_MEM || avgSYSSwapUsed >= SYS_MAX_SWAP)
	{
		MEMConflict = true;
#if (SHOWINFO == 1)
		printk("\033[33mMEM: %3d\tSWAP: %3d\t\033[0m", avgSYSMemUsed, avgSYSSwapUsed);
#endif
	}
	else
	{
		MEMConflict = false;
#if (SHOWINFO == 1)
		printk("MEM: %3d\tSWAP: %3d\t", avgSYSMemUsed, avgSYSSwapUsed);
#endif
	}

	getSysDiskUsedInfo();
	getSysNetUsedInfo();
	//judge whether IO resource is conflicting
	currentDiskUsedInfo = beginDiskUsedInfo;
	while(currentDiskUsedInfo != NULL)
	{
		if(currentDiskUsedInfo->ioUsed >= SYS_MAX_IO)
		{
			IOConflict = true;
#if (SHOWINFO == 1)
			printk("\033[33m%10s:%3d\t\033[0m", currentDiskUsedInfo->diskName, currentDiskUsedInfo->ioUsed);
#endif
		}
		else
		{
			IOConflict = false;
#if (SHOWINFO == 1)
			printk("%10s:%3d\t", currentDiskUsedInfo->diskName, currentDiskUsedInfo->ioUsed);
#endif
		}
		currentDiskUsedInfo = currentDiskUsedInfo->next;
	}

	//judge whether NET resource is conflicting
	currentNetUsedInfo = beginNetUsedInfo;
	while(currentNetUsedInfo != NULL)
	{
		if(currentNetUsedInfo->netUsed >= SYS_MAX_NET)
		{
			NETConflict = true;
#if (SHOWINFO == 1)
			printk("\033[33m%10s:%3d\t\033[0m", currentNetUsedInfo->netCardName, currentNetUsedInfo->netUsed);
#endif
		}
		else
		{
			NETConflict = false;
#if (SHOWINFO == 1)
			printk("%10s:%3d\t", currentNetUsedInfo->netCardName, currentNetUsedInfo->netUsed);
#endif
		}
		currentNetUsedInfo = currentNetUsedInfo->next;
	}
	printk("\n");
	//当系统资源使用超过预设值时，则认为系统可能会发生资源竞争
	if(CPUConflict || MEMConflict || IOConflict || NETConflict)
		return true;
	else
		return false;
}

bool judgeSoftWareConflict()
{
	//获取用户层所有程序的资源使用情况以及延时情况
	getUserLayerAPP();

	//更新系统的每类资源是否发生竞争
	//一段时间内系统的资源平均使用情况
	int sumCPU = 0, sumMem = 0, sumSwap = 0;
	int i;
	for(i = 0; i < MAX_RECORD_LENGTH; i++)
	{
		sumCPU += sysResArray[i].cpuUsed;
		sumMem += sysResArray[i].memUsed;
		sumSwap += sysResArray[i].swapUsed;
	}
	avgSYSCpuUsed = sumCPU/MAX_RECORD_LENGTH;
	avgSYSMemUsed = sumMem/MAX_RECORD_LENGTH;
	avgSYSSwapUsed = sumSwap/MAX_RECORD_LENGTH;
	//judge whether CPU resource is conflicting
	if(avgSYSCpuUsed >= SYS_MAX_CPU)
	{
		CPUConflict = true;
#if (SHOWINFO == 1)
		printk("\033[33mCPU: %3d\t\033[0m", avgSYSCpuUsed);
#endif
	}
	else
	{
		CPUConflict = false;
#if (SHOWINFO == 1)
		printk("CPU: %3d\t", avgSYSCpuUsed);
#endif
	}
	//judge whether MEM resource is conflicting
	if(avgSYSMemUsed >= SYS_MAX_MEM || avgSYSSwapUsed >= SYS_MAX_SWAP)
	{
		MEMConflict = true;
#if (SHOWINFO == 1)
		printk("\033[33mMEM: %3d\tSWAP: %3d\t\033[0m", avgSYSMemUsed, avgSYSSwapUsed);
#endif
	}
	else
	{
		MEMConflict = false;
#if (SHOWINFO == 1)
		printk("MEM: %3d\tSWAP: %3d\t", avgSYSMemUsed, avgSYSSwapUsed);
#endif
	}

	getSysDiskUsedInfo();
	getSysNetUsedInfo();
	//judge whether IO resource is conflicting
	currentDiskUsedInfo = beginDiskUsedInfo;
	while(currentDiskUsedInfo != NULL)
	{
		if(currentDiskUsedInfo->ioUsed >= SYS_MAX_IO)
		{
			IOConflict = true;
#if (SHOWINFO == 1)
			printk("\033[33m%10s:%3d\t\033[0m", currentDiskUsedInfo->diskName, currentDiskUsedInfo->ioUsed);
#endif
		}
		else
		{
			IOConflict = false;
#if (SHOWINFO == 1)
			printk("%10s:%3d\t", currentDiskUsedInfo->diskName, currentDiskUsedInfo->ioUsed);
#endif
		}
		currentDiskUsedInfo = currentDiskUsedInfo->next;
	}

	//judge whether NET resource is conflicting
	currentNetUsedInfo = beginNetUsedInfo;
	while(currentNetUsedInfo != NULL)
	{
		if(currentNetUsedInfo->netUsed >= SYS_MAX_NET)
		{
			NETConflict = true;
#if (SHOWINFO == 1)
			printk("\033[33m%10s:%3d\t\033[0m", currentNetUsedInfo->netCardName, currentNetUsedInfo->netUsed);
#endif
		}
		else
		{
			NETConflict = false;
#if (SHOWINFO == 1)
			printk("%10s:%3d\t", currentNetUsedInfo->netCardName, currentNetUsedInfo->netUsed);
#endif
		}
		currentNetUsedInfo = currentNetUsedInfo->next;
	}
	printk("\n");

	int j;
	int aveWait_sum = 0;
	int aveIOWait_sum = 0;
	unsigned long aveMaj_flt_sum = 0;
	bool isConflict = false;
	currentMonitorAPP = beginMonitorAPP;
	while(currentMonitorAPP != NULL)
	{
		if(currentMonitorAPP->flags)
		{
			aveWait_sum = aveIOWait_sum = 0;
			aveMaj_flt_sum = 0;

			for(j = 0; j < MAX_RECORD_LENGTH; j++)
			{
				aveWait_sum += currentMonitorAPP->schedInfo[j].wait_sum;
				aveIOWait_sum += currentMonitorAPP->schedInfo[j].iowait_sum;
				aveMaj_flt_sum += currentMonitorAPP->maj_flt[j];
			}
			aveWait_sum /= MAX_RECORD_LENGTH;
			aveIOWait_sum /= MAX_RECORD_LENGTH;
			aveMaj_flt_sum /= MAX_RECORD_LENGTH;
			//printk("processName:%s \t wait_sum:%d \t iowait_sum:%d \t pageFaultNum: %ld\n", currentMonitorAPP->name, aveWait_sum, aveIOWait_sum, aveMaj_flt_sum);
			//if(strcmp(currentMonitorAPP->name, "redis-server") == 0 || strcmp(currentMonitorAPP->name, "mysqld") == 0 || strcmp(currentMonitorAPP->name, "apache2") == 0)
			//	printk("wait_sum:%d \t iowait_sum:%d \t pageFaultNum: %ld\n", aveWait_sum, aveIOWait_sum, aveMaj_flt_sum);

			if(aveWait_sum >= PROC_MAX_SCHED.wait_sum || aveIOWait_sum >= PROC_MAX_SCHED.iowait_sum || aveMaj_flt_sum >= PROC_MAX_MAJ_FLT)
			{
				isConflict = true;
				break;
			}
		}
		currentMonitorAPP = currentMonitorAPP->next;
	}
	
	/*
	 * set process resource usage when the resource contention is not occuring
	 */
	if(! isConflict)
	{
		/*
		 * 不存在软件延时
		 * 更新正常运行时软件资源使用情况(前一时刻的软件资源使用情况)
		 */
		currentMonitorAPP = beginMonitorAPP;
		int sumCPU, sumMEM, sumSWAP;
		unsigned long long sumIOData, sumNetData;
		int count;
		while(currentMonitorAPP != NULL)
		{
			if(currentMonitorAPP->flags)
			{
				sumCPU = sumMEM = sumSWAP = 0;
				sumIOData = sumNetData = 0;
				for(count = 0; count < MAX_RECORD_LENGTH; count++)
				{
					sumCPU += currentMonitorAPP->cpuUsed[count];
					sumMEM += currentMonitorAPP->memUsed[count];
					sumSWAP += currentMonitorAPP->swapUsed[count];
					sumIOData += currentMonitorAPP->ioDataBytes[count];
					sumNetData += currentMonitorAPP->netTotalBytes[count];
				}

				currentMonitorAPP->normalResUsed.cpuUsed = sumCPU / MAX_RECORD_LENGTH;
				currentMonitorAPP->normalResUsed.memUsed = sumMEM / MAX_RECORD_LENGTH;
				currentMonitorAPP->normalResUsed.swapUsed = sumSWAP / MAX_RECORD_LENGTH;
				currentMonitorAPP->normalResUsed.ioDataBytes = sumIOData / MAX_RECORD_LENGTH;
				currentMonitorAPP->normalResUsed.netTotalBytes = sumNetData / MAX_RECORD_LENGTH;
			}

			currentMonitorAPP = currentMonitorAPP->next;
		}
	}

	return isConflict;
}
