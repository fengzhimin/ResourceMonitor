/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-07 12:27
* Last modified: 2017-02-14 13:20
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

ProcInfo MonitorProcInfo[MAX_MONITOR_SOFTWARE_NUM];

int currentRecordSchedIndex = 0;                 //记录当前操作的proSchedInfo数组下标(把结构体ProcSchedInfoArray中的procSchedInfo数组看做为一个循环列表来操作)

ProcSchedInfoArray MonitorProcInfoArray[MAX_MONITOR_SOFTWARE_NUM];

int max_CPUUSE = 70;
int max_MEMUSE = 70;
int max_IOUSE = 80;
int max_NETUSE = 70;

int monitorNum = 0; 
