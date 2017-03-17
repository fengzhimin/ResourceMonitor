/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-04 12:35
* Last modified: 2016-11-12 12:23
* Email        : 374648064@qq.com
* Filename     : fileOper.c
* Description  : 
******************************************************/

#include "common/fileOper.h"
#include "common/strOper.h"
#include "log/logOper.h"
#include <errno.h>
#include "config.h"

FILE *OpenFile(const char* fileName, const char* mode)
{
	FILE *fd;
	fd = fopen(fileName, mode);

	return fd;
}

int WriteFile(FILE *fd, char *data)
{
	int _ret_value;
	size_t _data_size;
	_data_size = strlen(data);
	_ret_value = fwrite(data, sizeof(char), _data_size, fd);
	fflush(fd);
	if(_ret_value != _data_size)
		return errno;
	else
		return -1;
}

int ReadFile(FILE *fd, char *data, size_t size)
{
	int _ret_value;
	_ret_value = fread(data, sizeof(char), size, fd);
	printf("%d\n", _ret_value);
	if(_ret_value < 1)
		return errno;
	else
		return -1;
}

int ReadLine(FILE *fd, char *data)
{
	char _ch;
	int n = 0;
	while((_ch = getc(fd)) != EOF)
	{
		if(n >= LINE_CHAR_MAX_NUM)
		{
			RecordLog("配置文件的一行数据大小超过预设大小!\n");
			return -1;
		}
		if(_ch == '\n')
			return -1;
		data[n++] = _ch;
	}

	return 0;
}

int CloseFile(FILE *fd)
{
	return fclose(fd);
}

void RemoveNote(char *fileName, char *fileNameCopy)
{
	FILE *fd = OpenFile(fileName, "r");
	if(fd == NULL)
	{
		char error_info[200];
		sprintf(error_info, "%s%s%s%s%s", "文件: ", fileName, " 打开失败！ 错误信息： ", strerror(errno), "\n");
		RecordLog(error_info);
		return ;
	}
	FILE *fdCopy = OpenFile(fileNameCopy, "w+");
	if(fd == NULL)
	{
		char error_info[200];
		sprintf(error_info, "%s%s%s%s%s", "创建文件: ", fileNameCopy, " 失败！ 错误信息： ", strerror(errno), "\n");
		RecordLog(error_info);
		return ;
	}
	char lineInfo[LINE_CHAR_MAX_NUM];
	while(!feof(fd))
	{
		memset(lineInfo, 0, LINE_CHAR_MAX_NUM);
		ReadLine(fd, lineInfo);
		if(!JudgeNote(lineInfo))
			WriteFile(fdCopy, lineInfo);	
	}

	CloseFile(fd);
	CloseFile(fdCopy);

}

