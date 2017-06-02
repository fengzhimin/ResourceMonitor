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

bool getConfValueByLabelAndKey(char *label, char *key, char **value)
{
	struct file *fd = KOpenFile(KCODE_CONFIG_PATH, O_RDONLY);
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
					strcpy(*value, subStr2[1]);
					KCloseFile(fd);
					return true;
				}
				continue;
			}
		}
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
	}

	KCloseFile(fd);
	return false;
}
