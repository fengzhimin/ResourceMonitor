/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : logOper.c
* Descripe     : record log
******************************************************/

#include "log/logOper.h"

char *CreateLogInfo(const char *logInfo, const char *file, const char* function, const int line)
{
	char _date[100];
	GetLocalTime(_date, 0);  //获取当前系统时间
	char _function[50];
	sprintf(_function, "%s%20s%s", " [函数:", function, "] ");    //获取正在执行的函数名
	char _file[50];
	sprintf(_file, "%s%15s%s", "[文件:", file, "] ");           //获取正在执行的文件名
	char _line[50];
	sprintf(_line, "%s%4d%s", "[行数:", line, "] ");          //获取正在执行的行数
	
	int _size = strlen(logInfo) + strlen(_date) + strlen(_function) + strlen(_file) + strlen(_line) + 20;
	char *mergeInfo = malloc(_size*sizeof(char));
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
#if OPENLOG
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
	struct stat buf;
	stat(logName, &buf);
	//判断日志文件是否超过最大值
	if(buf.st_size >= MAX_LOG_FILE_SIZE)
	{
		remove(logName);
	}
	int _fd = OpenFile(logName, O_APPEND | O_RDWR);
	if(-1 == _fd)
		return -1;
	
	char *_mergeInfo = CreateLogInfo(logInfo, file, function, line);

//判断是否在终端显示信息
#if (SHOWINFO == 1)
	switch(rank)
	{
	case 0:
		//红色
		printf("\033[31m%s\033[0m", _mergeInfo);
		break;
	case 1:
		//黄色
		printf("\033[33m%s\033[0m", _mergeInfo);
		break;
	case 2:
	default:
		//绿色
		printf("\033[32m%s\033[0m", _mergeInfo);
	}
#endif

	int _ret_write = WriteFile(_fd, _mergeInfo);

	free(_mergeInfo);
	CloseFile(_fd);

	if(_ret_write != -1)
		return 0;
#endif

	return -1;
}
