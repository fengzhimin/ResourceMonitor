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
#include <linux/string.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>

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

#endif
