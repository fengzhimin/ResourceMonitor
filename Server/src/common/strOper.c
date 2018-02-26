/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-11 09:02
* Last modified: 2017-05-02 18:10
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
	int i;
	for(i = 0; i < _length; i++)
	{
		if(_ch[i] < '0' || _ch[i] > '9')
			return false;
	}

	return true;
}

unsigned long long ExtractNumFromStr(char *_str)
{
	int _str_length = strlen(_str);
	unsigned long long _ret = 0;
	int i;
	for(i = 0; i < _str_length; i++)
		if(_str[i] >= '0' && _str[i] <= '9')
			_ret = _ret*10 + _str[i] - '0';

	if(_ret >= 0)
		return _ret;
	else
		return -1;
}

int cutStrByLabelDebug(char *str, char ch, char subStr[][MAX_SUBSTR], int subStrLength, const char *file, const char *function, const int line)
{
	//将subStr清空
	int i;
	for(i = 0; i < subStrLength; i++)
		memset(subStr[i], 0, MAX_SUBSTR);

	int _strLength = strlen(str);
	char *pstr = &str[0];
	int _ret_subNum = 0;
	int j = 0;   //为上一个子字符串的最后一个字符的index+1
	for(i = 0; i < _strLength; i++)
	{
		if(str[i] == ch)
		{
			if((i-j-1) >= MAX_SUBSTR)
			{
				WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
				Error("子字符串的长度超过最大存放子串数组的大小!\n");
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
		WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
		Error("子字符串的长度超过最大存放子串数组的大小!\n");
		strncpy(subStr[_ret_subNum], pstr, MAX_SUBSTR-1);
	}
	else
		strcpy(subStr[_ret_subNum], pstr);

	return _ret_subNum+1;
}

void removeBeginSpace(char *str)
{
	int _length = strlen(str);
	char *temp = kmalloc(sizeof(char)*(_length+1), GFP_ATOMIC);
	memset(temp, 0, _length);
	strcpy(temp, str);
	memset(str, 0, _length);
	char *pstr = &temp[0];
	int i;
	for(i = 0; i < _length; i++)
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
	kfree(temp);
}

void removeChar(char *str, char ch)
{
	int _length = strlen(str);
	char *temp = kmalloc(sizeof(char)*(_length+1), GFP_ATOMIC);
	memset(temp, 0, _length);
	strcpy(temp, str);
	memset(str, 0, _length);
	int j = 0;
	int i;
	for(i = 0; i < _length; i++)
	{
		if(temp[i] == ch)
			continue;
		else
			str[j++] = temp[i];
	}

	kfree(temp);
}

void removeSpace(char *str)
{
	int _length = strlen(str);
	char *temp = kmalloc(sizeof(char)*(_length+1), GFP_ATOMIC);
	memset(temp, 0, _length);
	strcpy(temp, str);
	memset(str, 0, _length);
	int j = 0;
	int i;
	for(i = 0; i < _length; i++)
	{
		if(temp[i] == '\t' || temp[i] == ' ')
			continue;
		else
			str[j++] = temp[i];
	}

	kfree(temp);
}

int GetSubStrNum(char *str, char *substr)
{
	int ret_num = 0;
	char *temp = str;
	while(temp != NULL)
	{
		temp = strstr(temp, substr);
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

char* IntToStr(char *str, int num)
{
	if(num < 10)
	{
		str[0] = num + '0';
		return ++str;
	}
	else
	{
		str = IntToStr(str, num/10);
		str[0] = num%10 + '0';
		return ++str;
	}
}

char* LongToStr(char *str, long long num)
{
	if(num < 10)
	{
		str[0] = num + '0';
		return ++str;
	}
	else
	{
		str = IntToStr(str, num/10);
		str[0] = num%10 + '0';
		return ++str;
	}
}

int StrToInt(char *str)
{
	int strLength = strlen(str);
	int i, retNum = 0;
	for(i = 0; i < strLength; i++)
	{
		retNum *= 10;
		retNum += str[i] - '0';
	}

	return retNum;
}

char *Dec2Hex(char *ch, int num)
{
	if(num < 16)
	{
		ch[0] = num >= 10? 'A'+num-10:'0'+num;
	}
	else
	{
		ch = Dec2Hex(ch, num/16);
		num %= 16;
		ch[0] = num >= 10? 'A'+num-10:'0'+num;
	}

	return ++ch;
}

int StrFloatToInt(char *strFloat)
{
	int strFloatLength = strlen(strFloat);
	int i, retNum = 0;
	for(i = 0; i < strFloatLength; i++)
	{
		if(strFloat[i] == '.')
			break;
		retNum *= 10;
		retNum += strFloat[i] - '0';
	}

	return retNum;
}
