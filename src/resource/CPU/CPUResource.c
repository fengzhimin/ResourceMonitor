/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-02 19:38
* Last modified: 2017-04-02 19:38
* Email        : 374648064@qq.com
* Filename     : CPUResource.c
* Description  : 
******************************************************/

#include "resource/CPU/CPUResource.h"

static char lineData[LINE_CHAR_MAX_NUM];

static char error_info[200];

bool getProcessCPUTimeDebug(pid_t pid, Process_Cpu_Occupy_t *processCpuTime, const char *file, const char *function, const int line)
{
	memset(processCpuTime, 0, sizeof(Process_Cpu_Occupy_t));
	struct task_struct *p = pid_task(find_vpid(pid), PIDTYPE_PID);
	if(p == NULL)
		return false;
	else
	{
		processCpuTime->pid = pid;
		task_lock(p);
		struct task_struct *t = p;
		cputime_t utime, stime;
		processCpuTime->utime = p->signal->utime;
		processCpuTime->stime = p->signal->stime;
		rcu_read_lock();
		do
		{
			task_cputime(t, &utime, &stime);
			processCpuTime->utime += utime;
			processCpuTime->stime += stime;
		}while_each_thread(p, t);
		rcu_read_unlock();
		processCpuTime->cutime = cputime_to_clock_t(p->signal->cutime);
		processCpuTime->cstime = cputime_to_clock_t(p->signal->cstime);
		processCpuTime->utime = cputime_to_clock_t(processCpuTime->utime);
		processCpuTime->stime = cputime_to_clock_t(processCpuTime->stime);
		task_unlock(p);
	}

	return true;
}

bool getTotalCPUTimeDebug(Total_Cpu_Occupy_t *totalCpuTime, const char *file, const char *function, const int line)
{
	memset(totalCpuTime, 0, sizeof(Total_Cpu_Occupy_t));
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	struct file *fp = KOpenFile("/proc/stat", O_RDONLY);
	if(fp == NULL)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", "/proc/stat", " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return false;
	}
	if(KReadLine(fp, lineData) == -1)
	{
		char name[30];
		sscanf(lineData, "%s %u %u %u %u", name, &totalCpuTime->user, &totalCpuTime->nice, &totalCpuTime->system, &totalCpuTime->idle);
		KCloseFile(fp);
		return true;
	}
	else
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", "/proc/stat", " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		KCloseFile(fp);
		return false;
	}
}
