/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-11 14:32
* Last modified: 2017-06-20 21:40
* Email        : 374648064@qq.com
* Filename     : monitorSoftWare.c
* Description  : 
******************************************************/

#include "running/monitorSoftWare.h"

void clearMonitor()
{
	currentRecordSchedIndex = 0;
	int i;
	for(i = 0; i < MAX_MONITOR_SOFTWARE_NUM; i++)
	{
		memset(&MonitorProcInfo[i], 0, sizeof(ProcInfo));
		memset(&MonitorProcInfoArray[i], 0, sizeof(ProcSchedInfoArray));
	}
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

void clearMonitorAPPName()
{
	currentMonitorAPPName = beginMonitorAPPName;
	while(currentMonitorAPPName != NULL)
	{
		beginMonitorAPPName = beginMonitorAPPName->next;
		vfree(currentMonitorAPPName);
		currentMonitorAPPName = beginMonitorAPPName;
	}

	endMonitorAPPName = NULL;
	//clear MonitorAPPNameNum 
	MonitorAPPNameNum = 0;
}

bool insertMonitorAPPName(MonitorAPPName obj)
{
	bool insertPoint = true;
	currentMonitorAPPName = beginMonitorAPPName;
	while(currentMonitorAPPName != NULL)
	{
		if(strcasecmp(obj.name, currentMonitorAPPName->name) == 0)
		{
			insertPoint = false;
			break;
		}

		currentMonitorAPPName = currentMonitorAPPName->next;
	}
	if(insertPoint)
	{
		//insert new MonitorAPPName obj into list
		endMonitorAPPName = endMonitorAPPName->next = vmalloc(sizeof(MonitorAPPName));
		*endMonitorAPPName = obj;
	}

	return insertPoint;
}

void getAllMonitorAPPName()
{
	//clear MonitorAPPNameNum 
	MonitorAPPNameNum = 0;
	MonitorAPPName temp;
	memset(&temp, 0, sizeof(MonitorAPPName));
	//associate list header and tail
	beginMonitorAPPName = endMonitorAPPName = currentMonitorAPPName = vmalloc(sizeof(MonitorAPPName));
	memset(currentMonitorAPPName, 0, sizeof(MonitorAPPName));
	struct task_struct *task, *p;
	struct list_head *ps;
	task = &init_task;
	list_for_each(ps, &task->tasks)
	{
		p = list_entry(ps, struct task_struct, tasks);
		//By judge process utimescaled to determine whether the program is user level or kernel level programm
		if(p->utimescaled > 0)
		{
			memset(&temp, 0, sizeof(MonitorAPPName));
			strcpy(temp.name, p->comm);
			if(insertMonitorAPPName(temp))
				MonitorAPPNameNum++;
		}
	}

	//in order to improve efficiency
	currentMonitorAPPName = beginMonitorAPPName;
	beginMonitorAPPName = beginMonitorAPPName->next;
	vfree(currentMonitorAPPName);
	currentMonitorAPPName = beginMonitorAPPName;
}
