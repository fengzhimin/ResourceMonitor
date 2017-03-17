/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-11 09:02
* Last modified: 2017-02-12 21:37
* Email        : 374648064@qq.com
* Filename     : strOper.c
* Description  : 
******************************************************/

#define _GNU_SOURCE   //避免编译strcasestr时出现警告

#include "common/strOper.h"
#include "common/fileOper.h"
#include "log/logOper.h"

bool isNum(char *_ch)
{
	int _length = strlen(_ch);
	for(int i = 0; i < _length; i++)
	{
		if(_ch[i] < '0' || _ch[i] > '9')
			return false;
	}

	return true;
}

int GetFirstCharIndex(char *_str)
{
	int _first_char_index = 0;
	int _str_length = strlen(_str);
	for(int i = 0; i < _str_length; i++)
		if(_str[i] == ' ' || _str[i] == '\t')    //空白字符和制表符
			_first_char_index++;
		else
			break;

	return _first_char_index;
}

bool JudgeNote(char *_str)
{
	int _first_char_index = GetFirstCharIndex(_str);

	int _note_num = GetNote_SymbolNum();
	for(int i = 0; i < _note_num; i++)
	{
		int note_size = strlen(note_symbol[i]);
		char temp[20];
		memset(temp, 0, 20);
		strncpy(temp, &(_str[_first_char_index]), note_size);
		if(strcasecmp(temp, note_symbol[i]) == 0)
			return true;
	}

	return false;
}

int GetFirstWordFromStr(char *_str, char *_first_word)
{
	int _str_length = strlen(_str);
	int _first_char_begin_index = GetFirstCharIndex(_str);
	int _first_char_end_index;
	for(_first_char_end_index = _first_char_begin_index; _first_char_end_index < _str_length; _first_char_end_index++)
	{
		if(_str[_first_char_end_index] == ' ' || _str[_first_char_end_index] == '\t' \
				|| _str[_first_char_end_index] == '=' || _str[_first_char_end_index] == '\n')
			break;
	}

	int word_size = _first_char_end_index - _first_char_begin_index;
	if(word_size == 0)
		return -1;

	if(word_size > CONFIG_KEY_MAX_NUM)
	{
		RecordLog("配置文件中第一行的首个单词的长度大于配置项key的最大长度!\n");
		return -1;
	}
	strncpy(_first_word, &_str[_first_char_begin_index], word_size);

	return _first_char_end_index;
}

bool GetConfigInfo(char *_str, char _type[][CONFIG_KEY_MAX_NUM], int _type_num, ConfigInfo *_configInfo)
{
	char _first_word[CONFIG_KEY_MAX_NUM];
	memset(_first_word, 0, CONFIG_KEY_MAX_NUM);
	int _next_char_begin_index = GetFirstWordFromStr(_str, _first_word);
	int _str_length = strlen(_str);
	if(_next_char_begin_index == -1)
		return false;
	for(int i = 0; i < _type_num; i++)
	{
		if(strcasestr(_first_word, _type[i]) != NULL)
		{
			int _temp_index = 0;
			bool temp_point = false;
			strcpy(_configInfo->key, _first_word);
			for(int j = _next_char_begin_index; j < _str_length; j++)
			{
				//跳过key与value中间的字符
				if(_str[j] == ' ' || _str[j] == '\t' || _str[j] == '=' || _str[j] == '\n')
				{
					if(temp_point)
						return true;
					else
						continue;
				}
				else
				{
					_configInfo->value[_temp_index++] = _str[j];
					temp_point = true;
				}
			}
			return true;
		}
	}

	return false;
}

int GetConfigInfoFromConfigFile(ConfigInfo _configInfo[], char _type[][CONFIG_KEY_MAX_NUM], int _type_num, char _configfilepath[][FILE_PATH_MAX_LENGTH], int _configfilepathNum)
{
	int _real_configInfoNum = 0;
	for(int i = 0; i < _configfilepathNum; i++)
	{
		FILE *fd = OpenFile(_configfilepath[i], "r");	
		if(fd == NULL)
		{
			char error_info[200];
			sprintf(error_info, "%s%s%s%s%s", "文件: ", _configfilepath[i], " 打开失败！ 错误信息： ", strerror(errno), "\n");
			RecordLog(error_info);
			return -1;
		}
		char lineInfo[LINE_CHAR_MAX_NUM];
		while(!feof(fd))
		{
			memset(lineInfo, 0, LINE_CHAR_MAX_NUM);
			ReadLine(fd, lineInfo);
			if(!JudgeNote(lineInfo))   //判断是否为注释行
			{
				if(GetConfigInfo(lineInfo, _type, _type_num, &_configInfo[_real_configInfoNum]))
					_real_configInfoNum++;
			}
		}

		CloseFile(fd);
	}

	return _real_configInfoNum;
}


int ExtractNumFromStr(char *_str)
{
	int _str_length = strlen(_str);
	int _ret = 0;
	for(int i = 0; i < _str_length; i++)
		if(_str[i] >= '0' && _str[i] <= '9')
			_ret = _ret*10 + _str[i] - '0';

	if(_ret > 0)
		return _ret;
	else
		return -1;
}

int cutStrByLabel(char *str, char ch, char subStr[][MAX_SUBSTR], int subStrLength)
{
	//将subStr清空
	for(int i = 0; i < subStrLength; i++)
		memset(subStr[i], 0, MAX_SUBSTR);

	int _strLength = strlen(str);
	char *pstr = &str[0];
	int _ret_subNum = 0;
	int j = 0;   //为上一个子字符串的最后一个字符的index+1
	for(int i = 0; i < _strLength; i++)
	{
		if(str[i] == ch)
		{
			if((i-j-1) >= MAX_SUBSTR)
			{
				RecordLog("子字符串的长度超过最大存放子串数组的大小!\n");
				strncpy(subStr[_ret_subNum], pstr, MAX_SUBSTR-1);
				_ret_subNum++;
				if(subStrLength == (_ret_subNum+1))    //判断要截取的子串个数是否小于存放子串的数组大小
				{
					j = i + 1;
					pstr = &str[j];
					break;
				}
			}
			else if(i != j)   //如果是连续的ch分隔字符则跳过
			{
				strncpy(subStr[_ret_subNum], pstr, i-j);
				_ret_subNum++;
				if(subStrLength == (_ret_subNum+1))    //判断要截取的子串个数是否小于存放子串的数组大小
				{
					j = i + 1;
					pstr = &str[j];
					break;
				}
			}
			
			j = i + 1;
			pstr = &str[j];
		}
	}

	//将最后一部分字符串拷贝出来
	if(strlen(pstr) >= (MAX_SUBSTR+1))
	{
		RecordLog("子字符串的长度超过最大存放子串数组的大小!\n");
		strncpy(subStr[_ret_subNum], pstr, MAX_SUBSTR-1);
	}
	else
		strcpy(subStr[_ret_subNum], pstr);

	return _ret_subNum+1;
}

void removeBeginSpace(char *str)
{
	int _length = strlen(str);
	char *temp = malloc(sizeof(char)*(_length+1));
	memset(temp, 0, _length);
	strcpy(temp, str);
	memset(str, 0, _length);
	char *pstr = &temp[0];
	for(int i = 0; i < _length; i++)
	{
		if(temp[i] == ' ' || temp[i] == '\t')
		{
			pstr = &temp[i+1];
			continue;
		}
		else
			break;
	}
	strcpy(str, pstr);
	free(temp);
}

void removeChar(char *str, char ch)
{
	int _length = strlen(str);
	char *temp = malloc(sizeof(char)*(_length+1));
	memset(temp, 0, _length);
	strcpy(temp, str);
	memset(str, 0, _length);
	int j = 0;
	for(int i = 0; i < _length; i++)
	{
		if(temp[i] == ch)
			continue;
		else
			str[j++] = temp[i];
	}

	free(temp);
}

int GetSubStrNum(char *str, char *substr)
{
	int ret_num = 0;
	char *temp = str;
	while(temp != NULL)
	{
		temp = strcasestr(temp, substr);
		if(temp != NULL)
		{
			++ret_num;   //查找到一个匹配的字符串
			++temp;
		}
		else
			break;
	}

	return ret_num;
}
