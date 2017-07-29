/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-27 15:03
* Last modified: 2017-07-29 05:49
* Email        : 374648064@qq.com
* Filename     : DevResource.h
* Description  : 
******************************************************/

#ifndef __DEVRESOURE_H__
#define __DEVREOURCE_H__

#include <linux/monitorResource.h>
#include <linux/task_io_accounting_ops.h>
#include <linux/sched.h>
#include "config.h"
#include "common/fileOper.h"
#include "common/strOper.h"
#include "log/logOper.h"

#define getProcessIOData(pid, processIOData)   getProcessIODataDebug(pid, processIOData, __FILE__, __FUNCTION__, __LINE__)
#define getDiskState(diskPath, diskStatInfo)  getDiskStateDebug(diskPath, diskStatInfo, __FILE__, __FUNCTION__, __LINE__)
#define getAllDiskState(beginDiskInfo)        getAllDiskStateDebug(beginDiskInfo, __FILE__, __FUNCTION__, __LINE__)

/************************************************
 * func: 获取进程的IO数据
 * return true = 成功　　　false = 失败
 * @para pid: process id
 * @para processIOData: 存放进程IO数据
************************************************/
bool getProcessIODataDebug(pid_t pid, Process_IO_Data *processIOData, const char *file, const char *function, const int line);

/***********************************************
 * func: 获取磁盘的状态
 * return true = 成功   false = 失败
 * @para diskPath: 磁盘信息的路径(/sys/block/sda/stat)
 * @para diskStatInfo: 存放磁盘的状态信息
***********************************************/
bool getDiskStateDebug(char *diskPath, DiskStat *diskStatInfo, const char *file, const char *function, const int line);

/***********************************************
 * func: 获取当前系统的所有磁盘的状态
 * return: >0 = 成功(磁盘的个数)   0 = 失败
 * @para beginDiskInfo: 存放返回值第一个磁盘的首地址(链表存放)的指针
***********************************************/
int getAllDiskStateDebug(DiskInfo **beginDiskInfo, const char *file, const char *function, const int line);

#endif
