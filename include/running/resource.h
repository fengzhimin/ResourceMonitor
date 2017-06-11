/******************************************************
* Author       : fengzhimin
* Create       : 2016-12-27 15:19
* Last modified: 2017-04-10 21:29
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
#include "common/confOper.h"
#include "log/logOper.h"
#include "running/schedTime.h"
#include "resource/memeory/memResource.h"
#include "resource/CPU/CPUResource.h"
#include "resource/device/DevResource.h"
#include "resource/network/netResource.h"
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>


/***********************************************
 * func: 获取当前所有/proc下的进程
 * return: 运行进程的个数
 * @para path: 存放获取后的所有路径
***********************************************/
int getProcAll(ProcPIDPath *path);

/**********************************************
 * func: 申请三维数组存放进程的信息
 * return: 用于存放数据的指针名(char ***)
 * @para oneSize: 三维数组的面数
 * @para secondSize: 三维数组的行数
 * @para thirdSize: 三维数组的列数
**********************************************/
char*** mallocResource(int oneSize, int secondSize, int thirdSize);

/**********************************************
 * func: 释放三维数组存放进程的信息
 * return: void
 * @para info: 用于存放数据的指针名(char ***)
 * @para oneSize: 三维数组的面数
 * @para secondSize: 三维数组的行数
**********************************************/
void freeResource(char ***info, int oneSize, int secondSize);

/**********************************************
 * func: 根据进程ID来查找该进程资源使用情况
 * return: true = 查找成功   false = 失败
 * @para id: 进程的ID
 * @para info: 存放该进程的资源使用情况
 * @para allInfo: 存放所有进程的资源使用情况
 * @para allProcNum: 所有进程的个数
**********************************************/
bool getInfoByID(int id, ProcInfo *info, ProcInfo allInfo[], int allProcNum);

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
 * return: 运行进程的个数
 * @para info: 存放进程信息的数据
 * @para totalResource: 系统的总资源使用率
************************************************/
int getProgressInfo(ProcInfo **info, SysResource *totalResource);

/***********************************************
 * func: 获取系统的资源使用情况
 * return: void
 * @para totalResource: 系统的资源使用情况
***********************************************/
void getSysResourceInfo(SysResource *totalResource);

/***********************************************
 * func: 获取要监控软件的资源使用情况
 * return: void
 * @para monitorNum: 监控软件的个数
 * *********************************************/
void getMonitorProgressInfo(int monitorNum);

#endif
