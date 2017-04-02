/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-02 19:35
* Last modified: 2017-04-02 19:35
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

/**********************************************
 * func: 获取CPU使用时间
 * return: -1 = 失败   1 = 成功
 * @para totalCpuTime: 存放CPU使用时间
**********************************************/
int getTotalCPUTime(Total_Cpu_Occupy_t *totalCpuTime);

/*********************************************
 * func: 获取程序CPU使用时间
 * return: -1 = 失败   1 = 成功
 * @para stat: 程序的stat路径
 * @para processCpuTime: 存放程序使用CPU时间
*********************************************/
int getProcessCPUTime(char *stat, Process_Cpu_Occupy_t *processCpuTime);

#endif
