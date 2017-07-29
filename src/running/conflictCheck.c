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
	//合并磁盘数据
	char ioUsedInfo[100] = { 0 };
	char netUsedInfo[100] = { 0 };
	IOUsedInfo *diskUsed = NULL;
	NetUsedInfo *netUsed = NULL;
	int avgCPUUsed = 0;
	int avgMEMUsed = 0;
	int i;
	for(i = 0; i < MAX_RECORD_LENGTH; i++)
	{
		diskUsed = sysResArray[i].ioUsed;
		netUsed = sysResArray[i].netUsed;
		strcat(ioUsedInfo, "[");
		while(diskUsed != NULL)
		{
			sprintf(ioUsedInfo, "%s %s:%d", ioUsedInfo, diskUsed->diskName, diskUsed->ioUsed);
			diskUsed = diskUsed->next;
		}
		strcat(ioUsedInfo, "] ");
		strcat(netUsedInfo, "[");
		while(netUsed != NULL)
		{
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
			netUsed = netUsed->next;
		}
		strcat(netUsedInfo, "] ");
	}
	int count = 0;
	int sum = 0;
	printk("总CPU使用率为: ");
	for(i = 0; i < MAX_RECORD_LENGTH; i++)
	{
		if(sysResArray[i].cpuUsed >= 0)
		{
			sum += sysResArray[i].cpuUsed;
			count++;
		}
		printk("[%d] ", sysResArray[i].cpuUsed);
	}
	printk("\n");
	avgCPUUsed = sum/count;
	sum = count = 0;
	printk("总MEM使用率为: ");
	for(i = 0; i < MAX_RECORD_LENGTH; i++)
	{
		if(sysResArray[i].memUsed > 0)
		{
			sum += sysResArray[i].memUsed;
			count++;
		}
		printk("[%d] ", sysResArray[i].memUsed);
	}
	printk("\n");
	avgMEMUsed = sum/count;
	printk("IO使用率: %s\n NET使用率: %s\n", ioUsedInfo, netUsedInfo);
	printk("CPU 平均使用率: %d\t内存平均使用率: %d\n", avgCPUUsed, avgMEMUsed);
	if(avgCPUUsed >= SYS_MAX_CPU || avgMEMUsed >= SYS_MAX_MEM)
		return true;
	else
		return false;
}

bool judgeSoftWareConflict()
{
	getUserLayerAPP();
	currentMonitorAPP = beginMonitorAPP;
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
			if(aveWait_sum >= PROC_MAX_SCHED.wait_sum || aveIOWait_sum >= PROC_MAX_SCHED.iowait_sum || true)
				return true;
		}
		currentMonitorAPP = currentMonitorAPP->next;
	}

	return false;
}
