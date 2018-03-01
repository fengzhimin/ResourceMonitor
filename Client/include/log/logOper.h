/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : logOper.h
* Descripe     : record log
******************************************************/

#ifndef __LOGRECORD_H__
#define __LOGRECORD_H__

#include <stdio.h>
#include <stdlib.h>
#include "common/fileOper.h"
#include "common/dateOper.h"
#include "config.h"

/**********************************
 * function: 宏定义实现默认参数函数
**********************************/
#define Error(logInfo)   WriteLog(0, ERROR_LOG_FILE, logInfo, __FILE__, __FUNCTION__, __LINE__)
#define Warning(logInfo)   WriteLog(1, WARNING_LOG_FILE, logInfo, __FILE__, __FUNCTION__, __LINE__)
#define Result(logInfo)   WriteLog(2, RESULT_LOG_FILE, logInfo, __FILE__, __FUNCTION__, __LINE__)

/**********************************
 *function: 合成需要记录的日志信息
 *return: 生成的日志信息
 *@para logInfo:　需要写入的日志信息
 *@para file: 为当前文件名称(__FILE)
 *@para function: 为当前函数名称(__FUNCTION__)
 *@para line: 为当前执行的行数(__LINE__)
**********************************/
char *CreateLogInfo(const char* logInfo, const char *file, const char *function, const int line);


/**********************************
 *function: 插入日志
 *return: -1＝成功　　0 = 失败
 *@para rank: 日志的级别
 *@para logName：日志的文件名
 *@para logInfo: 日志的信息
 *@para file: 为当前文件名称(__FILE)
 *@para function: 为当前函数名称(__FUNCTION__)
 *@para line: 为当前执行的行数(__LINE__)
**********************************/
int WriteLog(int rank, const char* logName, const char* logInfo, const char *file, const char *function, const int line);

#endif
