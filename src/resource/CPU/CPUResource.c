/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-02 19:38
* Last modified: 2017-07-30 05:59
* Email        : 374648064@qq.com
* Filename     : CPUResource.c
* Description  : 
******************************************************/

#include "resource/CPU/CPUResource.h"

static u64 get_idle_time(int cpu)
{
	u64 idle, idle_time = -1ULL;

	if (cpu_online(cpu))
		idle_time = get_cpu_idle_time_us(cpu, NULL);

	if (idle_time == -1ULL)
		/* !NO_HZ or cpu offline so we can rely on cpustat.idle */
		idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
	else
		idle = usecs_to_cputime64(idle_time);

	return idle;
}

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

	int i;
	u64 user, nice, system, idle;

	user = nice = system = idle  = 0;

	for_each_possible_cpu(i) {
		user += kcpustat_cpu(i).cpustat[CPUTIME_USER];
		nice += kcpustat_cpu(i).cpustat[CPUTIME_NICE];
		system += kcpustat_cpu(i).cpustat[CPUTIME_SYSTEM];
		idle += get_idle_time(i);
	}

	totalCpuTime->user = cputime64_to_clock_t(user);
	totalCpuTime->nice = cputime64_to_clock_t(nice);
	totalCpuTime->system = cputime64_to_clock_t(system);
	totalCpuTime->idle = cputime64_to_clock_t(idle);

	return true;
}
