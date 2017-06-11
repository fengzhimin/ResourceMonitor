/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-11 14:32
* Last modified: 2017-06-11 14:32
* Email        : 374648064@qq.com
* Filename     : monitorSoftWare.c
* Description  : 
******************************************************/

#include "running/monitorSoftWare.h"

void clearMonitor()
{
	int i;
	for(i = 0; i < MAX_MONITOR_SOFTWARE_NUM; i++)
		memset(&MonitorProcInfo[i], 0, sizeof(ProcInfo));
}

void clearMonitorExceptName(int monitorNum)
{
	int i;
	char tempName[MAX_INFOLENGTH];
	for(i = 0; i < monitorNum; i++)
	{
		strcpy(tempName, MonitorProcInfo[i].name);
		memset(&MonitorProcInfo[i], 0, sizeof(ProcInfo));
		strcpy(MonitorProcInfo[i].name, tempName);
	}
}
