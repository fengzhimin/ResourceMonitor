/******************************************************
* Author       : fengzhimin
* Create       : 2016-12-27 15:19
* Last modified: 2017-04-10 21:29
* Email        : 374648064@qq.com
* Filename     : resource.h
* Description  : 获取软件占用系统资源
******************************************************/

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "config.h"
#include "common/fileOper.h"
#include "common/dirOper.h"
#include "common/strOper.h"
#include "common/confOper.h"
#include "log/logOper.h"
#include "running/schedTime.h"
#include "running/monitorSoftWare.h"
#include "running/process.h"
#include "resource/memeory/memResource.h"
#include "resource/CPU/CPUResource.h"
#include "resource/device/DevResource.h"
#include "resource/network/netResource.h"
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>

/***********************************************
 * func: 获取系统的资源使用情况
 * return: void
***********************************************/
void getSysResourceInfo();

/***********************************************
 * func: get all user layer APP
 * return: void
***********************************************/
void getUserLayerAPP();

#endif
