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
