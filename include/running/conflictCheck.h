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
 * func: 解决进程之间关联的问题
 * return: void
 * @para info: 存放最终进程的资源数据
 * @para processNum: 进程的个数
***********************************************/
void solveProcessRelate(ProcInfo info[], int processNum);

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
