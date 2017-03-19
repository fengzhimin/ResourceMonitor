/******************************************************
* Author       : fengzhimin
* Create       : 2016-12-27 15:19
* Last modified: 2017-02-23 18:51
* Email        : 374648064@qq.com
* Filename     : resource.h
* Description  : 获取软件占用系统资源
******************************************************/

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "config.h"
#include "common/fileOper.h"
#include "common/dirOper.h"
#include "common/strOper.h"
#include "log/logOper.h"
#include <linux/delay.h>

/***********************************************
 * func: 查找软件name的在/proc目录下的status文件路径
 * return: -1 = 错误    0 = 没有找到对于软件的status文件    1 = 找到
 * @para name: 软件名
 * @para path: 存放status的路径
 * example path = /proc/1580
************************************************/
int getStatusPathByName(char name[], char path[]);

/***********************************************
 * func: 获取应用程序使用系统资源的情况
 * return: -1 = 错误   1 = 成功
 * @para path: 存放status的路径
 * @para info: 存放资源使用信息
 *		 info[0] = 程序名称
 *		 info[1] = 程序PID
 *		 info[2] = 程序PPID
 *		 info[3] = 程序CPU使用率
 *		 info[4] = 程序MEM使用率
 *		 info[5] = 程序使用虚拟内存大小
 *		 info[6] = 程序使用物理内存大小
 *		 info[7] = 程序状态
************************************************/
int getProgressInfo(char path[], char info[][MAX_INFOLENGTH]);


/**********************************************
 * func: 获取物理内存大小
 * return: -1 = 失败   1 = 成功
 * @para totalMem: 存放物理内存大小
**********************************************/
int getTotalPM(char totalMem[]);

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
