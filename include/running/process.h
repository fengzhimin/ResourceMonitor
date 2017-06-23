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
#include "resource/CPU/CPUResource.h"
#include "resource/memeory/memResource.h"
#include "resource/device/DevResource.h"
#include "resource/network/netResource.h"
#include "running/schedTime.h"
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

/***********************************************
 * func: get a program all VmRSS
 * return: return program all VmRSS   -1 = failure
 * @para pidArray: The pid Array of a program
************************************************/
unsigned int getProgramVmRSS(int *pidArray);

/***********************************************
 * func: get a program all CPU time
 * return: return ProgAllRes object and set ProgAllRes cpuTime and flags
 * @para progName: program name
 * @para pidArray: The pid Array of a program
************************************************/
ProgAllRes getProgramCPU(char *progName, int *pidArray);

/***********************************************
 * func: get a program all IO data
 * return: return ProgAllRes object and set ProgAllRes ioDataBytes and flags
 * @para progName: program name
 * @para pidArray: The pid Array of a program
************************************************/
ProgAllRes getProgramIOData(char *progName, int *pidArray);

/***********************************************
 * func: get a program all Sched info
 * return: return ProgAllRes object and set ProgAllRes shcedInfo and flags
 * @para progName: program name
 * @para pidArray: The pid Array of a program
************************************************/
ProgAllRes getProgramSched(char *progName, int *pidArray);

/***********************************************
 * func: get a program all net data
 * return: return netTotalBytes  -1 = failure
 * @para pidArray: The pid Array of a program
************************************************/
unsigned long long getProgramNetData(int *pidArray);

#endif
