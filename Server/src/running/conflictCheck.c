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

	int i;
	//一段时间内系统的资源平均使用情况
	int sumCPU = 0, sumMem = 0, sumSwap = 0;
	for(i = 0; i < MAX_RECORD_LENGTH; i++)
	{
		sumCPU += sysResArray[i].cpuUsed;
		sumMem += sysResArray[i].memUsed;
		sumSwap += sysResArray[i].swapUsed;
	}
	avgSYSCpuUsed = sumCPU/MAX_RECORD_LENGTH;
	avgSYSMemUsed = sumMem/MAX_RECORD_LENGTH;
	avgSYSSwapUsed = sumSwap/MAX_RECORD_LENGTH;
	getSysDiskUsedInfo();
	getSysNetUsedInfo();
	bool ret = false;
	printk("CPU: %3d\tMEM: %3d\tSWAP: %3d\t", avgSYSCpuUsed, avgSYSMemUsed, avgSYSSwapUsed);
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
	//当系统资源使用超过预设值时，则认为系统可能会发生资源竞争
	if(avgSYSCpuUsed >= SYS_MAX_CPU || avgSYSMemUsed >= SYS_MAX_MEM || avgSYSSwapUsed >= SYS_MAX_SWAP || ret)
		return true;
	else
		return false;
}

bool judgeSoftWareConflict()
{
	//获取用户层所有程序的资源使用情况以及延时情况
	getUserLayerAPP();

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
		 * 更新正常运行时软件资源使用情况
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
