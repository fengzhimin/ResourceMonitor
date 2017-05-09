/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-02 19:05
* Last modified: 2017-04-02 19:05
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

/**********************************************
 * func: 获取物理内存大小
 * return: false = 失败   true = 成功
 * @para totalMem.memTotal: 存放物理内存总大小
 * @para totalMem.memAvailable: 存放物理内存可用大小
**********************************************/
bool getTotalPM(MemInfo *totalMem);

#endif
