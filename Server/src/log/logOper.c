/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-04 12:47
* Last modified: 2017-03-17 18:14
* Email        : 374648064@qq.com
* Filename     : logOper.c
* Description  : 
******************************************************/

#include "log/logOper.h"
#include "common/fileOper.h"
#include "common/dateOper.h"
#include "config.h"


char *CreateLogInfo(const char *logInfo, const char *file, const char* function, const int line)
{
	char _date[100];
	GetLocalTime(_date, 0);  //获取当前系统时间
	char _function[50];
	sprintf(_function, "%s%20s%s", " [函数:", function, "] ");    //获取正在执行的函数名
	char _file[50];
	sprintf(_file, "%s%25s%s", "[文件:", file, "] ");           //获取正在执行的文件名
	char _line[50];
	sprintf(_line, "%s%4d%s", "[行数:", line, "] ");          //获取正在执行的行数
	
	int _size = strlen(logInfo) + strlen(_date) + strlen(_function) + strlen(_file) + strlen(_line) + 20;
	char *mergeInfo = kmalloc(_size*sizeof(char), GFP_ATOMIC);
	memset(mergeInfo, 0, _size);
	strcat(mergeInfo, "[");
	strcat(mergeInfo, _date);
	strcat(mergeInfo, "]  ");
	strcat(mergeInfo, _file);
	strcat(mergeInfo, _function);
	strcat(mergeInfo, _line);
	strcat(mergeInfo, "  操作：");
	strcat(mergeInfo, logInfo);

	return mergeInfo;
}

int WriteLog(int rank, const char* logInfo, const char *file, const char* function, const int line)
{
#if (OPENLOG == 1)
	char *logName = NULL;
	switch(rank)
	{
	case 0:
		logName = ERROR_LOG_FILE;
		break;
	case 1:
		logName = WARNING_LOG_FILE;
		break;
	case 2:
	default:
		logName = RESULT_LOG_FILE;
	}
	struct file * _fd = KOpenFile(logName, O_APPEND | O_WRONLY);
	if(NULL == _fd)
		return -1;
	
	char *_mergeInfo = CreateLogInfo(logInfo, file, function, line);

//判断是否在终端显示信息
#if (SHOWINFO == 1)
	switch(rank)
	{
	case 0:
		//红色
		printk("\033[31m%s\033[0m", _mergeInfo);
		break;
	case 1:
		//黄色
		printk("\033[33m%s\033[0m", _mergeInfo);
		break;
	case 2:
	default:
		//绿色
		printk("\033[32m%s\033[0m", _mergeInfo);
	}
#endif

	int _ret_write = KWriteFile(_fd, _mergeInfo);

	kfree(_mergeInfo);
	KCloseFile(_fd);

	if(_ret_write != -1)
		return 0;
#endif

	return -1;
}
