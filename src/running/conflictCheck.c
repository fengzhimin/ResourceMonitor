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
	getSysResourceInfo();
	int avgCPUUsed = 0;
	int avgMEMUsed = 0;
	int avgSwapUsed = 0;
	int i;
	int sumCPU = 0, sumMem = 0, sumSwap = 0;
	for(i = 0; i < MAX_RECORD_LENGTH; i++)
	{
		sumCPU += sysResArray[i].cpuUsed;
		sumMem += sysResArray[i].memUsed;
		sumSwap += sysResArray[i].swapUsed;
	}
	avgCPUUsed = sumCPU/MAX_RECORD_LENGTH;
	avgMEMUsed = sumMem/MAX_RECORD_LENGTH;
	avgSwapUsed = sumSwap/MAX_RECORD_LENGTH;
	getSysDiskUsedInfo();
	getSysNetUsedInfo();
	bool ret = false;
	printk("CPU 平均使用率: %3d\t内存平均使用率: %3d\t", avgCPUUsed, avgMEMUsed);
	currentDiskUsedInfo = beginDiskUsedInfo;
	for(i = 0; i < currentDiskNum; i++)
	{
		printk("%10s:%3d\t", currentDiskUsedInfo->diskName, currentDiskUsedInfo->ioUsed);
		if(currentDiskUsedInfo->ioUsed >= SYS_MAX_IO)
			ret = true;
		currentDiskUsedInfo = currentDiskUsedInfo->next;
	}
	currentNetUsedInfo = beginNetUsedInfo;
	for(i = 0; i < currentNetNum; i++)
	{
		printk("%10s:%3d\t", currentNetUsedInfo->netCardName, currentNetUsedInfo->netUsed);
		if(currentNetUsedInfo->netUsed >= SYS_MAX_NET)
			ret = true;
		currentNetUsedInfo = currentNetUsedInfo->next;
	}
	printk("\n");
	if(avgCPUUsed >= SYS_MAX_CPU || avgMEMUsed >= SYS_MAX_MEM || avgSwapUsed >= SYS_MAX_SWAP || ret)
		return true;
	else
		return false;
}

bool judgeSoftWareConflict()
{
	getUserLayerAPP();
	int j;
	int aveWait_sum = 0;
	int aveIOWait_sum = 0;
	currentMonitorAPP = beginMonitorAPP;
	while(currentMonitorAPP != NULL)
	{
		if(currentMonitorAPP->flags)
		{
			for(j = 0; j < MAX_RECORD_LENGTH; j++)
			{
				aveWait_sum += currentMonitorAPP->schedInfo[j].wait_sum;
				aveIOWait_sum += currentMonitorAPP->schedInfo[j].iowait_sum;
			}
			aveWait_sum /= MAX_RECORD_LENGTH;
			aveIOWait_sum /= MAX_RECORD_LENGTH;
			//当1s内的等待时间大于500ms时认为软件有冲突
			if(aveWait_sum >= PROC_MAX_SCHED.wait_sum || aveIOWait_sum >= PROC_MAX_SCHED.iowait_sum)
				return true;
		}
		currentMonitorAPP = currentMonitorAPP->next;
	}

	return false;
}
