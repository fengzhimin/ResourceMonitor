/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-02 19:38
* Last modified: 2017-04-02 19:38
* Email        : 374648064@qq.com
* Filename     : CPUResource.c
* Description  : 
******************************************************/

#include "resource/CPU/CPUResource.h"

static char stat_data[1000];
static char subStr18[18][MAX_SUBSTR];
static char lineData[LINE_CHAR_MAX_NUM];

static char error_info[200];

bool getProcessCPUTimeDebug(char *stat, Process_Cpu_Occupy_t *processCpuTime, const char *file, const char *function, const int line)
{
	memset(processCpuTime, 0, sizeof(Process_Cpu_Occupy_t));
	struct file *fp = KOpenFile(stat, O_RDONLY);
	if(fp == NULL)
	{
		/*
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", stat, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		*/
		return false;
	}

	memset(stat_data, 0, 1000);
	//if(KReadLine(fp, stat_data) == -1)
	int size = KReadFile(fp, stat_data, 1000);
	if(size > 0)
	{
		//printk("size = %d\n", size);
		cutStrByLabel(stat_data, ' ', subStr18, 18);
		processCpuTime->pid = ExtractNumFromStr(subStr18[12]);
		processCpuTime->utime = ExtractNumFromStr(subStr18[13]);
		processCpuTime->stime = ExtractNumFromStr(subStr18[14]);
		processCpuTime->cutime = ExtractNumFromStr(subStr18[15]);
		processCpuTime->cstime = ExtractNumFromStr(subStr18[16]);
		KCloseFile(fp);
		return true;
	}
	else
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", stat, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		KCloseFile(fp);
		return false;
	}
}

bool getTotalCPUTimeDebug(Total_Cpu_Occupy_t *totalCpuTime, const char *file, const char *function, const int line)
{
	memset(totalCpuTime, 0, sizeof(Total_Cpu_Occupy_t));
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	struct file *fp = KOpenFile("/proc/stat", O_RDONLY);
	if(fp == NULL)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", "/proc/stat", " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return false;
	}
	if(KReadLine(fp, lineData) == -1)
	{
		char name[30];
		sscanf(lineData, "%s %u %u %u %u", name, &totalCpuTime->user, &totalCpuTime->nice, &totalCpuTime->system, &totalCpuTime->idle);
		KCloseFile(fp);
		return true;
	}
	else
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", "/proc/stat", " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		KCloseFile(fp);
		return false;
	}
}
