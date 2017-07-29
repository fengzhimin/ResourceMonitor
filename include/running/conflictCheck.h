/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-11 09:33
* Last modified: 2017-04-11 09:33
* Email        : 374648064@qq.com
* Filename     : conflictCheck.h
* Description  : 对资源的冲突进行检测
******************************************************/

#ifndef __CONFLICTCHECK_H__
#define __CONFLICTCHECK_H__

#include "running/resource.h"

/***********************************************
 * func: 判断是否系统资源冲突
 * return: false = 不冲突   true = 冲突
***********************************************/
bool judgeSysResConflict();

/***********************************************
 * func: 判断是否监控软件使用资源冲突
 * return: false = 没有冲突   true = 有冲突
***********************************************/
bool judgeSoftWareConflict();

#endif
