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

bool getConfValueByLabelAndKeyDebug(char *label, char *key, char *value, const char *file, const char *function, const int line)
{
	struct file *fd = KOpenFile(KCODE_CONFIG_PATH, O_RDONLY);
	if(fd == NULL)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", KCODE_CONFIG_PATH, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return false;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	bool point = false;
	char *temp_str = NULL;
	while(KReadLine(fd, lineData) == -1)
	{
		removeBeginSpace(lineData);
		if(lineData[0] != KCODE_CONFIG_NOTESYMBOL)
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

int getMonitorSoftWareDebug(const char *file, const char *function, const int line)
{
	struct file *fd = KOpenFile(KCODE_CONFIG_PATH, O_RDONLY);
	if(fd == NULL)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", KCODE_CONFIG_PATH, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return 0;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	bool point = false;
	int retMonitorNum = 0;
	char *temp_str = NULL;
	while(KReadLine(fd, lineData) == -1)
	{
		removeBeginSpace(lineData);
		if(lineData[0] != KCODE_CONFIG_NOTESYMBOL)
		{
			if(lineData[0] == '[' && lineData[strlen(lineData)-1] == ']')
			{
				if(point)
					break;
				//判断是监控软件label
				lineData[strlen(lineData)-1] = '\0';
				temp_str = &(lineData[1]);
				if(strcasecmp(temp_str, MONITOR_LABEL) == 0)
				{
					memset(lineData, 0, LINE_CHAR_MAX_NUM);
					point = true;
					continue;
				}
			}
			if(point)
			{
				//是要查找的监控软件label域
				//判断不是注释行，则提取key与value
				cutStrByLabel(lineData, '=', subStr2, 2);
				removeChar(subStr2[0], ' ');
				//判断是否为要被提取的配置项
				if(strcasecmp(MONITOR_KEY, subStr2[0]) == 0)
				{
					if(retMonitorNum == MAX_MONITOR_SOFTWARE_NUM)
					{
						WriteLog("logInfo.log", "调用者信息\n", file, function, line);
						RecordLog("配置文件中的监控软件个数大于预定义监控软件个数(MAX_MONITOR_SOFTWARE_NUM)\n");
						break;
					}
					removeChar(subStr2[1], ' ');
					strcpy(MonitorProcInfoArray[retMonitorNum].procName, subStr2[1]);
					strcpy(MonitorProcInfo[retMonitorNum].name, subStr2[1]);
					retMonitorNum++;
				}
			}
		}
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
	}

	KCloseFile(fd);
	return retMonitorNum;
}

