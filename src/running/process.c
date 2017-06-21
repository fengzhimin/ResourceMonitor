/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-21 19:51
* Last modified: 2017-06-21 19:51
* Email        : 374648064@qq.com
* Filename     : process.c
* Description  : 
******************************************************/

#include "running/process.h"

static char error_info[200];

void clearMonitorProgPid()
{
	currentMonitorProgPid = beginMonitorProgPid;
	while(currentMonitorProgPid != NULL)
	{
		beginMonitorProgPid = beginMonitorProgPid->next;
		vfree(currentMonitorProgPid);
		currentMonitorProgPid = beginMonitorProgPid;
	}

	endMonitorProgPid = NULL;
}

ProgAllPid getAllPidDebug(char *name, const char *file, const char *function, const int line)
{
	ProgAllPid ret;
	memset(&ret, 0, sizeof(ProgAllPid));
	strcpy(ret.name, name);
	int recordPidIndex = 0;

	struct task_struct *task, *p;
	struct list_head *ps;
	task = &init_task;
	list_for_each(ps, &task->tasks)
	{
		p = list_entry(ps, struct task_struct, tasks);
		if(strcasecmp(p->comm, name) == 0)
		{
			if(recordPidIndex == MAX_CHILD_PROCESS_NUM)
			{
				WriteLog("logInfo.log", "调用者信息\n", file, function, line);
				sprintf(error_info, "%s%s", "程序(%s)的进程个数超过了预定义最大进程个数\n", name);
				RecordLog(error_info);
				
				return ret;
			}
			ret.pid[recordPidIndex++] = p->pid;
		}
	}

	return ret;
}

void getAllMonitorProgPid()
{
	if(MonitorAPPNameNum == 0)
		return ;
	beginMonitorProgPid = endMonitorProgPid = currentMonitorProgPid = vmalloc(sizeof(ProgAllPid));
	memset(beginMonitorProgPid, 0, sizeof(ProgAllPid));
	*beginMonitorProgPid = getAllPid(beginMonitorAPPName->name);

	currentMonitorAPPName = beginMonitorAPPName->next;
	while(currentMonitorAPPName != NULL)
	{
		endMonitorProgPid = endMonitorProgPid->next = vmalloc(sizeof(ProgAllPid));
		memset(endMonitorProgPid, 0, sizeof(ProgAllPid));
		*endMonitorProgPid = getAllPid(currentMonitorAPPName);
		currentMonitorAPPName = currentMonitorAPPName->next;
	}
}
