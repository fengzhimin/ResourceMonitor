/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-02 19:35
* Last modified: 2017-06-02 19:25
* Email        : 374648064@qq.com
* Filename     : CPUResource.h
* Description  : 
******************************************************/
#ifndef __CPURESOURCE_H__
#define __CPURESOURCE_H__

#include "config.h"
#include "common/fileOper.h"
#include "common/strOper.h"
#include "log/logOper.h"
#include <linux/sched.h>
#include <linux/kernel_stat.h>
#include <linux/tick.h>

#define getProcessCPUTime(pid, processCpuTime)  getProcessCPUTimeDebug(pid, processCpuTime, __FILE__, __FUNCTION__, __LINE__)
#define getTotalCPUTime(totalCpuTime)            getTotalCPUTimeDebug(totalCpuTime, __FILE__, __FUNCTION__, __LINE__)

/**********************************************
 * func: 获取CPU使用时间
 * return true = 成功　　　false = 失败
 * @para totalCpuTime: 存放CPU使用时间
**********************************************/
bool getTotalCPUTimeDebug(Total_Cpu_Occupy_t *totalCpuTime, const char *file, const char *function, const int line);

/*********************************************
 * func: 获取程序CPU使用时间
 * return true = 成功　　　false = 失败
 * @para pid: process id
 * @para processCpuTime: 存放程序使用CPU时间
*********************************************/
bool getProcessCPUTimeDebug(pid_t pid, Process_Cpu_Occupy_t *processCpuTime, const char *file, const char *function, const int line);

#endif
