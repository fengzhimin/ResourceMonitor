/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-09 20:44
* Last modified: 2017-06-09 20:44
* Email        : 374648064@qq.com
* Filename     : schedTime.h
* Description  : 关于进程调度(/proc/pid/sched)
******************************************************/

#ifndef __SCHEDTIME_H__
#define __SCHEDTIME_H__

#include "config.h"
#include "common/fileOper.h"
#include "common/strOper.h"
#include "log/logOper.h"

#define getProcSchedInfo(sched) getProcSchedInfoDebug(sched, __FILE__, __FUNCTION__, __LINE__)


/**************************************
 * func: 获取一个进程的ProcSchedInfo结构体数据
 * return: 返回ProcSchedInfo结构体
 * @para sched: 进程的sched路径
**************************************/
ProcSchedInfo getProcSchedInfoDebug(char *sched, const char *file, const char *function, const int line);

#endif
