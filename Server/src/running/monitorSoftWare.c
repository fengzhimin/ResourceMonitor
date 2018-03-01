/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-11 14:32
* Last modified: 2017-06-20 21:40
* Email        : 374648064@qq.com
* Filename     : monitorSoftWare.c
* Description  : 
******************************************************/

#include "running/monitorSoftWare.h"

static char buffer[PAGE_SIZE];

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
		//judge whether a process has be monitored or not
		if(strcasecmp(obj.name, currentMonitorAPPName->name) == 0 && obj.pgid == currentMonitorAPPName->pgid)
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
	int ret_cmdlineSize = 0;
	pid_t pgid;
	task = &init_task;
	list_for_each(ps, &task->tasks)
	{
		p = list_entry(ps, struct task_struct, tasks);
		//task_lock(p);
		pgid = getPgid(p);
		memset(buffer, 0, PAGE_SIZE);
	    ret_cmdlineSize = get_cmdline(p, buffer, PAGE_SIZE);
		//By judge process cmdline whether space to determine whether the program is user level or kernel level programm
		if(ret_cmdlineSize > 0)
		{
			memset(&temp, 0, sizeof(MonitorAPPName));
			strcpy(temp.name, p->comm);
			temp.pgid = pgid;
			if(insertMonitorAPPName(temp))
				MonitorAPPNameNum++;
		}
		//task_unlock(p);
	}

	//in order to improve efficiency
	currentMonitorAPPName = beginMonitorAPPName;
	beginMonitorAPPName = beginMonitorAPPName->next;
	vfree(currentMonitorAPPName);
}