/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-11 10:02
* Last modified: 2017-04-11 10:02
* Email        : 374648064@qq.com
* Filename     : conflictCheck.c
* Description  : 
******************************************************/

#include "running/conflictCheck.h"

void solveProcessRelate(ProcInfo info[], int processNum)
{
	int i, temp;
	bool ret;
	//char parentInfo[PROCESS_INFO_NUM][MAX_INFOLENGTH];
	ProcInfo parentInfo;
	for(i = processNum-1; i >= 0; i--)
	{
		temp = info[i].cpuUsed;
		//当进程的CPU使用率符合需求时,查找其对应的父进程和父父进程的资源使用情况
		if(temp >= PROCESSRELATECPUDOWN && temp <= PROCESSRELATECPUUP)
		{
			//获取其父进程的资源使用情况
			ret = getInfoByID(info[i].ppid, &parentInfo, info, processNum);
			if(ret)
			{
				info[i].cpuUsed += parentInfo.cpuUsed;
				info[i].memUsed += parentInfo.memUsed;
				//获取父父进程
				ret = getInfoByID(parentInfo.ppid, &parentInfo, info, processNum);
				if(ret)
				{
					info[i].cpuUsed += parentInfo.cpuUsed;
					info[i].memUsed += parentInfo.memUsed;
				}
			}
		}
	}
}

bool judgeSysResConflict(SysResource sysResource)
{
	//合并磁盘数据
	char ioUsedInfo[100] = { 0 };
	char netUsedInfo[100] = { 0 };
	IOUsedInfo *diskUsed = NULL;
	NetUsedInfo *netUsed = NULL;
	while(sysResource.ioUsed != NULL)
	{
		diskUsed = sysResource.ioUsed;
		sysResource.ioUsed = sysResource.ioUsed->next;
		sprintf(ioUsedInfo, "%s %s:%d", ioUsedInfo, diskUsed->diskName, diskUsed->ioUsed);
		vfree(diskUsed);
	}
	while(sysResource.netUsed != NULL)
	{
		netUsed = sysResource.netUsed;
		sysResource.netUsed = sysResource.netUsed->next;
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
	printk("总CPU使用率为: %d\t总内存使用率为: %d\t IO使用率: %s\t NET使用率: %s\n", sysResource.cpuUsed, sysResource.memUsed, ioUsedInfo, netUsedInfo);

	if(sysResource.cpuUsed >= max_CPUUSE || sysResource.memUsed >= max_MEMUSE)
		return true;
	else
		return false;
}

bool judgeSoftWareConflict()
{
	getUserLayerAPP();
	int i, j;
	int aveWait_sum = 0;
	int aveIOWait_sum = 0;
	currentMonitorAPP = beginMonitorAPP;
	for(i = 0; i < MonitorAPPNameNum; i++)
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
			if(aveWait_sum >= 500 || aveIOWait_sum >= 500)
				return true;
		}
		currentMonitorAPP = currentMonitorAPP->next;
	}

	return false;
}
