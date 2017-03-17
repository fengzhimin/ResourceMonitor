/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-04 08:52
* Last modified: 2016-11-04 13:06
* Email        : 374648064@qq.com
* Filename     : dateOper.h
* Description  : 时间操作函数头文件
******************************************************/

#ifndef __DATEOPER_H__
#define __DATEOPER_H__

#include <stdio.h>
#include <time.h>

/**********************************************
* function: 获取当前系统时间
* return：当前时间－0　　-1－失败
* @para buf: 存储当前时间
* @para fmt: 返回时间字符串的格式
**********************************************/
int GetLocalTime(char *buf, int fmt);

#endif

