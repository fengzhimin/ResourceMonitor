/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-02 19:08
* Last modified: 2017-04-02 19:08
* Email        : 374648064@qq.com
* Filename     : memResource.c
* Description  : 
******************************************************/

#include "resource/memeory/memResource.h"

static char lineData[LINE_CHAR_MAX_NUM];

static char error_info[200];

bool getTotalPMDebug(MemInfo *totalMem, const char *file, const char *function, const int line)
{
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	int lineNum = 1;
	struct file *fp = KOpenFile("/proc/meminfo", O_RDONLY);
	char subStr[2][MAX_SUBSTR];
	if(fp == NULL)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", "/proc/meminfo", " 失败！ 错误信息： ", "   ", "\n");
		RecordLog(error_info);
		return false;
	}
	while(KReadLine(fp, lineData) == -1)
	{
		if(lineNum == 1)
		{
			//提取/proc/meminfo 中的第一行数据(MemTotal)
			cutStrByLabel(lineData, ':', subStr, 2);
			totalMem->memTotal = ExtractNumFromStr(subStr[1]);
		}
		else if(lineNum == 3)
		{
			//提取/proc/meminfo 中的第三行数据(MemAvailable)
			cutStrByLabel(lineData, ':', subStr, 2);
			totalMem->memAvailable = ExtractNumFromStr(subStr[1]);
			break;
		}
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
		lineNum++;
	}
	if(lineNum == 1)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", "/proc/meminfo", " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		KCloseFile(fp);
		return false;
	}
	KCloseFile(fp);
	return true;
}

