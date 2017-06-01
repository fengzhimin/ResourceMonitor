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

/***************************************
 * func: 读取配置项的值
 * return: true = 读取成功   false = 读取失败
 * @para key: 配置项的名称
 * @para value: 存放配置项的值
***************************************/
bool getConfValueByKey(char *key, char **value);

#endif
