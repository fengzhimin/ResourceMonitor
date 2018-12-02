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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ExecuteCommand(command)   ExecuteCommandDebug(command, __FILE__, __FUNCTION__, __LINE__)
#define ReduceConf(softwareName, confName)  ReduceConfDebug(softwareName, confName, __FILE__, __FUNCTION__, __LINE__)
#define IncreaseConf(softwareName, confName, increaseValue, defValue)   IncreaseConfDebug(softwareName, confName, increaseValue, defValue, __FILE__, __FUNCTION__, __LINE__)
#define RecordTunedConfInfo(softwareName, confName, resourceType)   RecordTunedConfInfoDebug(softwareName, confName, resourceType, __FILE__, __FUNCTION__, __LINE__)
#define UpdateTunedConfInfo(softwareName, confName)   UpdateTunedConfInfoDebug(softwareName, confName, __FILE__, __FUNCTION__, __LINE__)
#define AutoIncreaseConf()   AutoIncreaseConfDebug(__FILE__, __FUNCTION__, __LINE__)

/**********************************
 * func: execute a command that modifies the resource-related configuration options
 * return: 
 *		-1 = 执行system函数失败
 *		-2 = 执行command命令失败
 *	   >=0 = 执行command命令成功，并返回command的状态码
 * @para command: 执行的命令，要求返回值为大于等于零的数字
**********************************/
int ExecuteCommandDebug(char *command, const char *file, const char *function, const int line);

/**********************************
 * func: execute a script that is reducing the value of configurations
 * return: true = execution success  false = execution failure
 * @para softwareName: the name of the software
 * @para confName: the name of configuration option
**********************************/
bool ReduceConfDebug(char *softwareName, char *confName, const char *file, const char *function, const int line);

/**********************************
 * func: execute a script that is increasing the value of configurations
 * return:
		0 = success
		1 = the value of config is equal to default value
		other = failure
 * @para softwareName: the name of the software
 * @para confName: the name of configuration option
 * @para increaseValue: increase the level of configuration option value
 * @para defValue: the default value of configuration option
***********************************/
int IncreaseConfDebug(char *softwareName, char *confName, char *increaseValue, char *defValue, const char *file, const char *function, const int line);

/**********************************
 * func: record the reduced config information
 * return: true = success    false = failure
 * @para softwareName: the name of the software
 * @para confName: the name of the reduced config
 * @para resourceType: the resource type
**********************************/
bool RecordTunedConfInfoDebug(char *softwareName, char *confName, char *resourceType, const char *file, const char *function, const int line);

/*********************************
 * func: automaic increase the value of configuration option
 * return: true = success    false = failure
*********************************/
bool AutoIncreaseConfDebug(const char *file, const char *function, const int line);

#endif
