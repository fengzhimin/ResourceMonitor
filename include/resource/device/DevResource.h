/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-27 15:03
* Last modified: 2017-04-27 15:03
* Email        : 374648064@qq.com
* Filename     : DevResource.h
* Description  : 
******************************************************/

#ifndef __DEVRESOURE_H__
#define __DEVREOURCE_H__

#include <linux/monitorResource.h>
#include "config.h"
#include "common/fileOper.h"
#include "common/strOper.h"
#include "log/logOper.h"

/************************************************
 * func: 获取进程的IO数据
 * return true = 成功　　　false = 失败
 * @para io: 进程的io文件路径
 * @para processIOData: 存放进程IO数据
************************************************/
bool getProcessIOData(char *io, Process_IO_Data *processIOData);

/***********************************************
 * func: 获取磁盘的状态
 * return true = 成功   false = 失败
 * @para diskPath: 磁盘信息的路径(/sys/block/sda/stat)
 * @para diskStatInfo: 存放磁盘的状态信息
***********************************************/
bool getDiskState(char *diskPath, DiskStat *diskStatInfo);

/***********************************************
 * func: 获取当前系统的所有磁盘的状态
 * return: >0 = 成功(磁盘的个数)   0 = 失败
 * @para beginDiskInfo: 存放返回值第一个磁盘的首地址(链表存放)的指针
***********************************************/
int getAllDiskState(DiskInfo **beginDiskInfo);

#endif
