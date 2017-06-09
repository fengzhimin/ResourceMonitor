/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-09 20:50
* Last modified: 2017-06-09 20:50
* Email        : 374648064@qq.com
* Filename     : schedTime.c
* Description  : 
******************************************************/

#include "running/schedTime.h"

static char subStr2[2][MAX_SUBSTR];
static char lineData[LINE_CHAR_MAX_NUM];
static char error_info[200];

ProcSchedInfo getProcSchedInfoDebug(char *sched, const char *file, const char *function, const int line)
{
	ProcSchedInfo retValue;
	memset(&retValue, 0, sizeof(ProcSchedInfo));
	struct file *fp = KOpenFile(sched, O_RDONLY);
	if(fp == NULL)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s", "打开文件: ", sched, "失败!\n");
	}
	else
	{
		while(KReadLine(fp, lineData) == -1)
		{
			removeChar(lineData, ' ');
			cutStrByLabel(lineData, ':', subStr2, 2);
			if(strcasecmp(subStr2[0], "se.sum_exec_runtime") == 0)
				retValue.sum_exec_runtime = StrFloatToInt(subStr2[1]);
			else if(strcasecmp(subStr2[0], "se.statistics.wait_sum") == 0)
				retValue.wait_sum = StrFloatToInt(subStr2[1]);
			else if(strcasecmp(subStr2[0], "se.statistics.iowait_sum") == 0)
			{
				retValue.iowait_sum = StrFloatToInt(subStr2[1]);
				break;
			}
		}
		KCloseFile(fp);
	}


	return retValue;
}
