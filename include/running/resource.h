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
#include "log/logOper.h"
#include "resource/memeory/memResource.h"
#include "resource/CPU/CPUResource.h"
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>


/***********************************************
 * func: 获取当前所有/proc下的进程
 * return: 运行进程的个数
 * @para path: 存放获取后的所有路径
***********************************************/
int getProcAll(ProcPIDPath *path);

/**********************************************
 * func: 申请三维数组存放进程的信息
 * return: 用于存放数据的指针名(char ***)
 * @para procNum: 申请进程的个数
**********************************************/
char*** mallocResource(int procNum);

/**********************************************
 * func: 释放三维数组存放进程的信息
 * return: void
 * @para info: 用于存放数据的指针名(char ***)
 * @para procNum: 申请进程的个数
**********************************************/
void freeResource(char ***info, int procNum);

/**********************************************
 * func: 根据进程ID来查找该进程资源使用情况
 * return: true = 查找成功   false = 失败
 * @para id: 进程的ID
 * @para info: 存放该进程的资源使用情况
 * @para allInfo: 存放所有进程的资源使用情况
 * @para allProcNum: 所有进程的个数
**********************************************/
bool getInfoByID(char *id, char info[][MAX_INFOLENGTH], char ***allInfo, int allProcNum);

/***********************************************
 * func: 查找软件name的在/proc目录下的status文件路径
 * return: -1 = 错误    0 = 没有找到对于软件的status文件    1 = 找到
 * @para name: 软件名
 * @para path: 存放status的路径
 * example path = /proc/1580
************************************************/
int getStatusPathByName(char name[], char path[]);

/***********************************************
 * func: 获取应用程序使用系统资源的情况
 * return: 运行进程的个数
 * @para info: 是一个四级指针,存放资源使用信息，是一个三维数组   第一维是进程个数   第二维是进程信息个数   第三维是信息对应的字符串
 *		 info[i][0] = 程序名称
 *		 info[i][1] = 程序PID
 *		 info[i][2] = 程序PPID
 *		 info[i][3] = 程序CPU使用率
 *		 info[i][4] = 程序MEM使用率
 *		 info[i][5] = 程序使用虚拟内存大小
 *		 info[i][6] = 程序使用物理内存大小
 *		 info[i][7] = 程序状态
 * @para totalResource:
 *		 totalResource[0] = 总CPU使用情况
 *		 totalResource[1] = 总内存使用情况
************************************************/
int getProgressInfo(char ****info, char totalResouce[][MAX_INFOLENGTH]);

#endif
