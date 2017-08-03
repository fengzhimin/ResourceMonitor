/******************************************************
* Author       : fengzhimin
* Create       : 2017-05-08 13:11
* Last modified: 2017-05-08 13:11
* Email        : 374648064@qq.com
* Filename     : config.h
* Description  : 
******************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <linux/types.h>

#define MAX_INFOLENGTH           50    //获取应用程序占用系统每个资源数据的最大长度

/*************************************
 * function: 存放关于进程调度的时间数值
*************************************/
typedef struct ProcessSchedInfo
{
	int sum_exec_runtime;   //累计运行的物理时间
	int wait_sum;     //在就绪队列里的等待时间
	int iowait_sum;   //io等待时间
} ProcSchedInfo;

/**********************************
 * function: 存放资源冲突的进程信息
 * name: process name
 * pgid: process group id
 * conflictType: 冲突的类型
 * next: 下一个地址
**********************************/
typedef struct ConflictProcess
{
	char name[MAX_INFOLENGTH];   //进程的名称
	pid_t pgid;
	int conflictType;
	struct ConflictProcess *next;
} ConflictProcInfo;

//每一位代表一种资源是否冲突   1 = 冲突   0 = 不冲突
#define CPU_CONFLICT    1    //CPU资源冲突(00000001)
#define MEM_CONFLICT    2    //MEM资源冲突(00000010)
#define NET_CONFLICT    4    //NET资源冲突(00000100)
#define IO_CONFLICT     8    //IO资源冲突 (00001000)
#define PORT_CONFLICT   16   //端口资源冲突(00010000)

#endif
