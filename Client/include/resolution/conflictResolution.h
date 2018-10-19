/******************************************************
* Author       : fengzhimin
* Create       : 2018-03-01 04:43
* Last modified: 2018-05-17 20:49
* Email        : 374648064@qq.com
* Filename     : conflictResolution.h
* Description  : the resolution of resource conflict
******************************************************/

#ifndef __CONFLICTRESOLUTION_H__
#define __CONFLICTRESOLUTION_H__

#include "common/strOper.h"
#include "log/logOper.h"
#include "common/confOper.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define ExecuteCommand(command)   ExecuteCommandDebug(command, __FILE__, __FUNCTION__, __LINE__)
#define ReduceConf(softwareName, confName)  ReduceConfDebug(softwareName, confName, __FILE__, __FUNCTION__, __LINE__)
#define IncreaseConf(softwareName, confName, defValue)   IncreaseConfDebug(softwareName, confName, defValue, __FILE__, __FUNCTION__, __LINE__)

/**********************************
 * func: execute a command that modifies the resource-related configuration options
 * return: true = execute success    false = execute failure
 * @para command: the command of execution
**********************************/
bool ExecuteCommandDebug(char *commandArgv[], const char *file, const char *function, const int line);

/**********************************
 * func: execute a script that is reducing the value of configurations
 * return: true = execution success  false = execution failure
 * @para softwareName: the name of the software
 * @para confName: the name of configuration option
**********************************/
bool ReduceConfDebug(char *softwareName, char *confName, const char *file, const char *function, const int line);

/**********************************
 * func: execute a script that is increasing the value of configurations
 * return: true = success    false = failure
 * @para softwareName: the name of the software
 * @para confName: the name of configuration option
 * @para defValue: the default value of configuration option
***********************************/
bool IncreaseConfDebug(char *softwareName, char *confName, char *defValue, const char *file, const char *function, const int line);

#endif
