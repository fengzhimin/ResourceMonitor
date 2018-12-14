/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-11 14:23
* Last modified: 2017-06-20 21:07
* Email        : 374648064@qq.com
* Filename     : monitorSoftWare.h
* Description  : 监控用户自定义软件
******************************************************/

#ifndef __MONITORSOFTWARE_H__
#define __MONITORSOFTWARE_H__

#include "config.h"
#include "common/fileOper.h"
#include "running/process.h"
#include "common/confOper.h"
#include <linux/string.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>

#define getMonitorSoftWare() getMonitorSoftWareDebug(__FILE__, __FUNCTION__, __LINE__)

/**********************************************
 * func: clear MonitorAPPName object
 * return: void
**********************************************/
void clearMonitorAPPName();

/*********************************************
 * func: insert MonitorAPPName object into MonitorAPPName list
 * return: false = always exist    true = insert into list
 * @para obj: will be inserted MonitorAPPName object
**********************************************/
bool insertMonitorAPPName(MonitorAPPName obj);

/*********************************************
 * func: get All user level running Program's name
 * return: void
*********************************************/
void getAllMonitorAPPName();

/****************************************
 * func: 从配置文件中获取要监控的软件(设置全局变量MonitorProcInfo)
 * return: 监控软件的个数
****************************************/
int getMonitorSoftWareDebug(const char *file, const char *function, const int line);

#endif
