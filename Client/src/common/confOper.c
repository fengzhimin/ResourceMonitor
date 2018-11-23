/******************************************************
* Author       : fengzhimin
* Create       : 2017-08-19 06:49
* Last modified: 2017-08-19 06:49
* Email        : 374648064@qq.com
* Filename     : confOper.c
* Description  : 
******************************************************/

#include "common/confOper.h"

static char lineData[LINE_CHAR_MAX_NUM];
static char subStr2[2][MAX_SUBSTR];
static char error_info[200];

bool getConfValueByLabelAndKeyDebug(char *label, char *key, char *value, \
		const char *file, const char *function, const int line)
{
	int fd = OpenFile(ResourceMonitor_Client_CONFIG_PATH, O_RDONLY);
	if(fd == -1)
	{
		WriteLog(0, "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", ResourceMonitor_Client_CONFIG_PATH, " 失败！ 错误信息： ", "   ", "\n");
		Error(error_info);
		return false;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	bool point = false;
	char *temp_str = NULL;
	while(ReadLine(fd, lineData))
	{
		removeBeginSpace(lineData);
		if(lineData[0] != ResourceMonitor_Client_CONFIG_NOTESYMBOL)
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
					//读取setCommand和getCommand配置项式可能会存在空格命令
					//removeChar(subStr2[1], ' ');
					if(strlen(subStr2[1]) >= CONFIG_VALUE_MAX_NUM)
					{
						CloseFile(fd);
						WriteLog(0, "调用者信息\n", file, function, line);
						Error("The value of config option is more than the max value\n");
						return false;
					}
					strcpy(value, subStr2[1]);
					CloseFile(fd);
					return true;
				}
			}
		}
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
	}

	CloseFile(fd);
	return false;
}
