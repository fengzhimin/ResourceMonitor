/******************************************************
* Author       : fengzhimin
* Create       : 2018-11-23 06:47
* Last modified: 2018-11-23 06:47
* Email        : 374648064@qq.com
* Filename     : procInfo.h
* Description  : 设置进程信息相关函数
******************************************************/

#ifndef __PROCINFO_H__
#define __PROCINFO_H__

#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include "config.h"
#include "log/logOper.h"

extern char **environ;

/******************************
 * func: 修改argv所指向的内存空间
 * return: void
 * @para argc: main原有参数
 * @para argv: main原有参数
 * @para envp: 全局环境变量
*******************************/
void setproctitle_init(int argc, char **argv, char **envp);

/******************************
 * func: 修改进程名称
 * return: void
 * @para fmt: 新的进程名称
*******************************/
void setproctitle(const char *fmt, ...);

/******************************
 * func: 查找一个进程的id(本身进程除外)
 * return: -1 = 执行函数出错   0 = 进程不存在   存在返回进程pid
 * @para procName: 进程名称
*******************************/
int getProcIdByName(char *procName);

#endif
