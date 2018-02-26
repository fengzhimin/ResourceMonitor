/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-09 20:50
* Last modified: 2017-06-09 20:50
* Email        : 374648064@qq.com
* Filename     : schedTime.c
* Description  : 
******************************************************/

#include "running/schedTime.h"

long long nsec_high(unsigned long long nsec)
{
	if((long long)nsec < 0)
	{
		nsec = -nsec;
		do_div(nsec, 1000000);
		return -nsec;
	}
	do_div(nsec, 1000000);

	return nsec;
}

unsigned long nsec_low(unsigned long long nsec)
{
	if((long long)nsec < 0)
		nsec = -nsec;

	return do_div(nsec, 1000000);
}

bool getProcSchedInfoDebug(pid_t pid, ProcSchedInfo *schedInfo, const char *file, const char *function, const int line)
{
	memset(schedInfo, 0, sizeof(ProcSchedInfo));
	struct task_struct *p = pid_task(find_vpid(pid), PIDTYPE_PID);
	if(p == NULL)
	{
		return false;
	}
	else
	{
		task_lock(p);
		schedInfo->sum_exec_runtime = nsec_high((long long)p->se.sum_exec_runtime);
		schedInfo->wait_sum = nsec_high((long long)p->se.statistics.wait_sum);
		schedInfo->iowait_sum = nsec_high((long long)p->se.statistics.iowait_sum);
		task_unlock(p);
		
		return true;
	}
}

ProcSchedInfo add(ProcSchedInfo value1, ProcSchedInfo value2)
{
	value1.sum_exec_runtime += value2.sum_exec_runtime;
	value1.wait_sum += value2.wait_sum;
	value1.iowait_sum += value2.iowait_sum;

	return value1;
}

ProcSchedInfo sub(ProcSchedInfo value1, ProcSchedInfo value2)
{
	value1.sum_exec_runtime -= value2.sum_exec_runtime;
	value1.wait_sum -= value2.wait_sum;
	value1.iowait_sum -= value2.iowait_sum;

	return value1;
}
