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

#include "config.h"
#include "common/fileOper.h"
#include "common/strOper.h"
#include "log/logOper.h"

/************************************************
 * func: 获取进程的IO数据
 * return -1 = 成功　　　1 = 失败
 * @para io: 进程的io文件路径
 * @para processIOData: 存放进程IO数据
************************************************/
int getProcessIOData(char *io, Process_IO_Data *processIOData);

#endif
