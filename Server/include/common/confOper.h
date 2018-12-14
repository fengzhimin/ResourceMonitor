/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-01 18:08
* Last modified: 2017-06-01 18:08
* Email        : 374648064@qq.com
* Filename     : confOper.h
* Description  : 操作配置文件
******************************************************/

#ifndef __CONFOPER_H__
#define __CONFOPER_H__

#include "fileOper.h"
#include "strOper.h"
#include "config.h"
#include "log/logOper.h"
#include "running/monitorSoftWare.h"

#define loadConfig() loadConfigDebug(__FILE__, __FUNCTION__, __LINE__)

#define getConfValueByLabelAndKey(label, key, value) getConfValueByLabelAndKeyDebug(label, key, value, __FILE__, __FUNCTION__, __LINE__)


/****************************************
 * func: load configuration from config file to initial the max value of Resource unilization
 * return: void
****************************************/
void loadConfigDebug(const char *file, const char *function, const int line);

/***************************************
 * func: 读取配置项的值
 * return: true = 读取成功   false = 读取失败
 * @para label: 配置项所在的标签
 * @para key: 配置项的名称
 * @para value: 存放配置项的值
 * example: [label1]
 *			key1 = value1
 *			key2 = value2
 *			[label2]
 *			key = value
 *			getConfValueByLabelAndKey("label1", "key1", &value)   --->  value = value1
***************************************/
bool getConfValueByLabelAndKeyDebug(char *label, char *key, char *value, const char *file, const char *function, const int line);

#endif
