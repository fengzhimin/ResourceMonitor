/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-21 19:30
* Last modified: 2017-06-21 19:30
* Email        : 374648064@qq.com
* Filename     : process.h
* Description  : get a process info
******************************************************/

#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "config.h"
#include "log/logOper.h"
#include <linux/string.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>

#define getAllPid(name) getAllPidDebug(name, __FILE__, __FUNCTION__, __LINE__)

/*************************************************
 * func: clear beginMonitorProgPid data
 * return: void
*************************************************/
void clearMonitorProgPid();

/*****************************************************
 * func: get a program all child process id
 * return: return ProgAllPid object
 * @para name: program name
*****************************************************/
ProgAllPid getAllPidDebug(char *name, const char *file, const char *function, const int line);

/************************************************
 * func: get All monitor program pid
 * return: void
************************************************/
void getAllMonitorProgPid();

#endif
