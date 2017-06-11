/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-11 14:23
* Last modified: 2017-06-11 14:23
* Email        : 374648064@qq.com
* Filename     : monitorSoftWare.h
* Description  : 监控用户自定义软件
******************************************************/

#ifndef __MONITORSOFTWARE_H__
#define __MONITORSOFTWARE_H__

#include "config.h"
#include <linux/string.h>


/***********************************************
 * func: 清空MonitorProcInfo数组
 * return: void
***********************************************/
void clearMonitor();

/**********************************************
 * func: 清空MonitorProcInfo数组中除了Name字段外的数据
 * return: void
 * @para monitorNum: MonitorProcInfo数据的前monitorNum项
**********************************************/
void clearMonitorExceptName(int monitorNum);

#endif
