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
#include <string.h>
#include <errno.h>

#define ExecuteCommand(command)   ExecuteCommandDebug(command, __FILE__, __FUNCTION__, __LINE__)
#define ResolveContention(softwareName, configName)   ResolveContentionDebug(softwareName, configName, __FILE__, __FUNCTION__, __LINE__)
#define getConfigValue(getCommand, value)  getConfigValueDebug(getCommand, value, __FILE__, __FUNCTION__, __LINE__)

/**********************************
 * func: execute a command that modifies the resource-related configuration options
 * return: true = execute success    false = execute failure
 * @para command: the command of how to modify the resource-related configuration options
**********************************/
bool ExecuteCommandDebug(char *command, const char *file, const char *function, const int line);

/**********************************
 * func: execute a command to get config value
 * return: true = execution success  false = execution failure
 * @para getCommand: the command that get config value
 * @para value: save config value
**********************************/
bool getConfigValueDebug(char *getCommand, char *value, const char *file, const char *function, const int line);

/**********************************
 * func: resolve the resource contention online
 * return: true = success    false = failure
 * @para softwareName: the name of the software
 * @para configFilePath: configuration file path
 * @para command: the command of how to modify the resource-related configuration options
 * @para configName: the name of configuration option
***********************************/
bool ResolveContentionDebug(char *softwareName, char *configName, const char *file, const char *function, const int line);

#endif
