/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : fileOper.c
* Descripe     : common file operation
******************************************************/

#include "common/fileOper.h"

int OpenFile(const char* fileName, int mode)
{
	int fd;
	mode |= O_CREAT;   //默认添加创建属性
	fd = open(fileName, mode, 0644);

	return fd;
}

int WriteFile(int fd, char *data)
{
	int _ret_value;

	_ret_value = write(fd, data, strlen(data));

	return _ret_value;
}

int WriteLine(int fd, char *data)
{
	int _ret_value;

	_ret_value = write(fd, data, strlen(data));

	_ret_value += write(fd, "\n", 1);

	return _ret_value;
}

int ReadFile(int fd, char *data, size_t size)
{
	int _ret_value;
	_ret_value = read(fd, data, size);

	return _ret_value;
}

int ReadLine(int fd, char *data)
{
	char _ch;
	int n = 0;
	while(ReadFile(fd, &_ch, 1) == 1)
	{
		if(n >= LINE_CHAR_MAX_NUM)
		{
			return -1;
		}
		if(_ch == '\n')
			return -1;
		data[n++] = _ch;
	}

	//read file last line
	if(n == 0)
		return 0;
	else
		return -1;
}

int CloseFile(int fd)
{
	return close(fd);
}
