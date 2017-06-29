/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-07 12:27
* Last modified: 2017-06-20 20:28
* Email        : 374648064@qq.com
* Filename     : config.c
* Description  : 用于定义全局变量
******************************************************/

#include "config.h"

char config_type[][20] = {".conf", ".config", ".cnf", ".config.in", ".conf.in"};   //判断配置文件的类型,每个类型字符个数不超过20


int GetConfig_TypeNum(void)
{
	return sizeof(config_type)/sizeof(config_type[0]);
}

//注释符号
char note_symbol[][10] = {"#", "%", "\""};

int GetNote_SymbolNum(void)
{
	return sizeof(note_symbol)/sizeof(note_symbol[0]);
}

Port_Map_Package *PortPackageData = NULL;
Port_Map_Package *beginPortPackageData = NULL;    //PortPackageData 第一个元素
Port_Map_Package *endPortPackageData = NULL;      //PortPackageData 最后一个元素
Port_Map_Package *currentPortPackageData = NULL;  //PortPackageData 当前操作的元素

ConflictProcInfo *beginConflictProcess = NULL;   //冲突信息的头
ConflictProcInfo *endConflictProcess = NULL;     //冲突信息的尾
ConflictProcInfo *currentConflictProcess = NULL; //当前的冲突信息

struct mutex ConflictProcess_Mutex;

int PROC_MAX_CPU = 30;
int PROC_MAX_MEM = 40;
unsigned long long PROC_MAX_IO = 102400000;   //10 M
unsigned long long PROC_MAX_NET = 1024000;   //1 M
ProcSchedInfo PROC_MAX_SCHED = 
{
	.sum_exec_runtime = 500,   //500 ms
	.wait_sum = 500,    //500 ms
	.iowait_sum = 500,  //500 ms
};

int SYS_MAX_CPU = 70;
int SYS_MAX_MEM = 70;
int SYS_MAX_IO = 80;
int SYS_MAX_NET = 70;

SysResource sysResArray[MAX_RECORD_LENGTH];

int currentRecordSysResIndex = 0;

MonitorAPPName *beginMonitorAPPName = NULL;     //用户层APP列表头
MonitorAPPName *endMonitorAPPName = NULL;       //用户层APP列表尾
MonitorAPPName *currentMonitorAPPName = NULL;   //当前操作的用户层APP

//系统用户层APP个数
int MonitorAPPNameNum = 0;

ResUtilization *beginMonitorAPP = NULL;
ResUtilization *endMonitorAPP = NULL;
ResUtilization *currentMonitorAPP = NULL;

//record current operate ResUtilization array index
int currentRecordResIndex = 0;

ProgAllPid *beginMonitorProgPid = NULL;
ProgAllPid *endMonitorProgPid = NULL;
ProgAllPid *currentMonitorProgPid = NULL;

