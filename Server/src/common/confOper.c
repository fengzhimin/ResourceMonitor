/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-01 18:15
* Last modified: 2017-06-01 18:15
* Email        : 374648064@qq.com
* Filename     : confOper.c
* Description  : 
******************************************************/

#include "common/confOper.h"

static char subStr2[2][MAX_SUBSTR];
static char lineData[LINE_CHAR_MAX_NUM];
static char error_info[200];

void loadConfigDebug(const char *file, const char *function, const int line)
{
	/*
	 * load system configuration
	 */
	char temp[CONFIG_VALUE_MAX_NUM];
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("SYSRESINFO", "CPU", temp))
	{
		SYS_MAX_CPU = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load SYSRESINFO-CPU value is failure!\n");
	}
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("SYSRESINFO", "MEM", temp))
	{
		SYS_MAX_MEM = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load SYSRESINFO-MEM value is failure!\n");
	}
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("SYSRESINFO", "SWAP", temp))
	{
		SYS_MAX_SWAP = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load SYSRESINFO-SWAP value is failure!\n");
	}
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("SYSRESINFO", "IO", temp))
	{
		SYS_MAX_IO = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load SYSRESINFO-IO value is failure!\n");
	}
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("SYSRESINFO", "NET", temp))
	{
		SYS_MAX_NET = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load SYSRESINFO-NET value is failure!\n");
	}

	/*
	 * load process configuration
	 */
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("PROCRESINFO", "CPU", temp))
	{
		PROC_MAX_CPU = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load PROCRESINFO-CPU value is failure!\n");
	}
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("PROCRESINFO", "MEM", temp))
	{
		PROC_MAX_MEM = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load PROCRESINFO-MEM value is failure!\n");
	}
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("PROCRESINFO", "MAJ_FLT", temp))
	{
		PROC_MAX_MAJ_FLT = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load PROCRESINFO-MAJ_FLT value is failure!\n");
	}
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("PROCRESINFO", "IO", temp))
	{
		PROC_MAX_IO = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load PROCRESINFO-IO value is failure!\n");
	}
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("PROCRESINFO", "NET", temp))
	{
		PROC_MAX_NET = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load PROCRESINFO-NET value is failure!\n");
	}
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("PROCRESINFO", "SCHED_SUM_EXEC_RUNTIME", temp))
	{
		PROC_MAX_SCHED.sum_exec_runtime = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load PROCRESINFO-SCHED.sum_exec_runtime value is failure!\n");
	}
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("PROCRESINFO", "SCHED_WAIT_SUM", temp))
	{
		PROC_MAX_SCHED.wait_sum = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load PROCRESINFO-SCHED.wait_sum value is failure!\n");
	}
	memset(temp, 0, CONFIG_VALUE_MAX_NUM);
	if(getConfValueByLabelAndKey("PROCRESINFO", "SCHED_IOWAIT_SUM", temp))
	{
		PROC_MAX_SCHED.iowait_sum = ExtractNumFromStr(temp);
	}
	else
	{
		WriteLog(1, "调用者信息\n", file, function, line);
		Warning("load PROCRESINFO-SCHED.iowait_sum value is failure!\n");
	}
}

bool getConfValueByLabelAndKeyDebug(char *label, char *key, char *value, const char *file, const char *function, const int line)
{
	struct file *fd = KOpenFile(ResourceMonitor_Server_CONFIG_PATH, O_RDONLY);
	if(fd == NULL)
	{
		WriteLog(0, "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", ResourceMonitor_Server_CONFIG_PATH, " 失败！ 错误信息： ", "    ", "\n");
		Error(error_info);
		return false;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	bool point = false;
	char *temp_str = NULL;
	while(KReadLine(fd, lineData) == -1)
	{
		removeBeginSpace(lineData);
		if(lineData[0] != ResourceMonitor_Server_CONFIG_NOTESYMBOL)
		{
			if(lineData[0] == '[' && lineData[strlen(lineData)-1] == ']')
			{
				if(point)
					break;
				//判断是label
				lineData[strlen(lineData)-1] = '\0';
				temp_str = &(lineData[1]);
				if(strcasecmp(temp_str, label) == 0)
				{
					memset(lineData, 0, LINE_CHAR_MAX_NUM);
					point = true;
					continue;
				}
			}
			if(point)
			{
				//是要查找的label域
				//判断不是注释行，则提取key与value
				cutStrByLabel(lineData, '=', subStr2, 2);
				removeChar(subStr2[0], ' ');
				//判断是否为要被提取的配置项
				if(strcasecmp(key, subStr2[0]) == 0)
				{
					removeChar(subStr2[1], ' ');
					if(strlen(subStr2[1]) >= CONFIG_VALUE_MAX_NUM)
					{
						WriteLog(0, "调用者信息\n", file, function, line);
						sprintf(error_info, "%s%s%s%d%s", "config key: the value of", key, "greater than", CONFIG_VALUE_MAX_NUM, "\n");
						Error(error_info);
						KCloseFile(fd);
						return false;
					}
					strcpy(value, subStr2[1]);
					KCloseFile(fd);
					return true;
				}
			}
		}
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
	}

	KCloseFile(fd);
	return false;
}

