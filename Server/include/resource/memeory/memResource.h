/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-02 19:05
* Last modified: 2017-08-16 05:35
* Email        : 374648064@qq.com
* Filename     : memResource.h
* Description  : 关于内存资源的监控
******************************************************/

#ifndef __MEMRESOURCE_H__
#define __MEMRESOURCE_H__

#include "config.h"
#include "common/fileOper.h"
#include "common/strOper.h"
#include "log/logOper.h"
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/mmzone.h>

#define getTotalPM(totalMem) getTotalPMDebug(totalMem, __FILE__, __FUNCTION__, __LINE__)

/**********************************************
 * func: 获取物理内存大小
 * return: false = 失败   true = 成功
 * @para totalMem.memTotal: 存放物理内存总大小
 * @para totalMem.memAvailable: 存放物理内存可用大小
**********************************************/
bool getTotalPMDebug(MemInfo *totalMem, const char *file, const char *function, const int line);

/**********************************************
 * func: get process main page fault number
 * return: maj_flt value
 * @para pid: process id
**********************************************/
unsigned long getProcessMAJ_FLT(pid_t pid);

/**********************************************
 * func: get process memory value
 * return: true = success  false = failure
 * @para pid: process id
 * @para memInfo: process memory Info
**********************************************/
bool getProcessMemInfo(pid_t pid, Process_Mem_Info *memInfo);

#endif
