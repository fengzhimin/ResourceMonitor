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

pid_t getPgid(struct task_struct *p)
{
	return pid_vnr(p->group_leader->pids[PIDTYPE_PGID].pid);
}

bool getProcessCmdline(struct task_struct *p, char *buffer)
{
	int ret = get_cmdline(p, buffer, PAGE_SIZE);
	if(ret > 0)
		return true;
	else		
		return false;
}

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

ProgAllPid getAllPidDebug(char *name, pid_t pgid, const char *file, const char *function, const int line)
{
	ProgAllPid ret;
	memset(&ret, 0, sizeof(ProgAllPid));
	strcpy(ret.name, name);
	ret.pgid = pgid;
	int recordPidIndex = 0;

	struct task_struct *task, *p;
	struct list_head *ps;
	task = &init_task;
	list_for_each(ps, &task->tasks)
	{
		p = list_entry(ps, struct task_struct, tasks);
		task_lock(p);
		//judge whether a process belong to program's child process by process group id or not
		if(strcasecmp(p->comm, name) == 0 && getPgid(p) == pgid)
		{
			if(recordPidIndex == MAX_CHILD_PROCESS_NUM)
			{
				WriteLog(0, "调用者信息\n", file, function, line);
				sprintf(error_info, "%s%s", "程序(%s)的进程个数超过了预定义最大进程个数\n", name);
				Error(error_info);
				task_unlock(p);
				return ret;
			}
			ret.childPid[recordPidIndex++] = p->pid;
		}
		task_unlock(p);
	}

	return ret;
}

void getAllMonitorProgPid()
{
	if(MonitorAPPNameNum == 0)
		return ;
	beginMonitorProgPid = endMonitorProgPid = currentMonitorProgPid = vmalloc(sizeof(ProgAllPid));
	memset(beginMonitorProgPid, 0, sizeof(ProgAllPid));
	*beginMonitorProgPid = getAllPid(beginMonitorAPPName->name, beginMonitorAPPName->pgid);

	currentMonitorAPPName = beginMonitorAPPName->next;
	while(currentMonitorAPPName != NULL)
	{
		endMonitorProgPid = endMonitorProgPid->next = vmalloc(sizeof(ProgAllPid));
		memset(endMonitorProgPid, 0, sizeof(ProgAllPid));
		*endMonitorProgPid = getAllPid(currentMonitorAPPName->name, currentMonitorAPPName->pgid);
		currentMonitorAPPName = currentMonitorAPPName->next;
	}
}

bool getProgramMemInfo(int *pidArray, Process_Mem_Info *memInfo)
{
	int i = 0;
	bool ret = false;
	Process_Mem_Info temp;
	memset(memInfo, 0, sizeof(Process_Mem_Info));
	for(; i < MAX_CHILD_PROCESS_NUM; i++)
	{
		if(pidArray[i] == 0)
			break;
		memset(&temp, 0, sizeof(Process_Mem_Info));
		if(getProcessMemInfo(pidArray[i], &temp))
		{
			ret = true;
			memInfo->rss += temp.rss;
			memInfo->swap += temp.swap;
		}
	}

	return ret;
}

ProgAllRes getProgramMaj_flt(char *progName, int *pidArray)
{
	int i = 0;
	ProgAllRes ret;
	memset(&ret, 0, sizeof(ProgAllRes));
	strcpy(ret.name, progName);

	for(; i < MAX_CHILD_PROCESS_NUM; i++)
	{
		if(pidArray[i] == 0)
			break;
		ret.maj_flt[i] = getProcessMAJ_FLT(pidArray[i]);
	}

	return ret;
}

ProgAllRes getProgramCPU(char *progName, int *pidArray)
{
	int i = 0;
	int ret_temp = -1;
	Process_Cpu_Occupy_t procCpuTime;
	ProgAllRes ret;
	memset(&ret, 0, sizeof(ProgAllRes));
	strcpy(ret.name, progName);

	for(; i < MAX_CHILD_PROCESS_NUM; i++)
	{
		if(pidArray[i] == 0)
			break;
		ret_temp = getProcessCPUTime(pidArray[i], &procCpuTime);
		if(ret_temp)
		{
			//valid
			ret.cpuTime[i] = procCpuTime.utime + procCpuTime.stime + procCpuTime.cutime + procCpuTime.cstime;
			ret.flags[i] = true;
		}
		else
			ret.flags[i] = false;   //invalid
	}

	return ret;
}

ProgAllRes getProgramIOData(char *progName, int *pidArray)
{
	int i = 0;
	bool ret_temp = false;
	Process_IO_Data procIOData;
	ProgAllRes ret;
	memset(&ret, 0, sizeof(ProgAllRes));
	strcpy(ret.name, progName);

	for(; i < MAX_CHILD_PROCESS_NUM; i++)
	{
		if(pidArray[i] == 0)
			break;
		ret_temp = getProcessIOData(pidArray[i], &procIOData);
		if(ret_temp)
		{
			//valid
			ret.ioDataBytes[i] = procIOData.read_bytes + procIOData.write_bytes;
			ret.flags[i] = true;
		}
		else
			ret.flags[i] = false;   //invalid
	}

	return ret;
}

ProgAllRes getProgramSched(char *progName, int *pidArray)
{
	int i = 0;
	ProcSchedInfo procSched;
	ProgAllRes ret;
	memset(&ret, 0, sizeof(ProgAllRes));
	strcpy(ret.name, progName);

	for(; i < MAX_CHILD_PROCESS_NUM; i++)
	{
		if(pidArray[i] == 0)
			break;
		if(getProcSchedInfo(pidArray[i], &procSched))
		{
			//valid
			ret.schedInfo[i] = procSched;
			ret.flags[i] = true;
		}
		else
			ret.flags[i] = false;   //invalid
	}

	return ret;
}
