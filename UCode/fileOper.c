/******************************************************
* Author       : fengzhimin
* Create       : 2017-08-19 07:04
* Last modified: 2017-08-19 07:04
* Email        : 374648064@qq.com
* Filename     : fileOper.c
* Description  : 
******************************************************/

#include "fileOper.h"

int OpenFile(char *pathname)
{
	int fd = open(pathname, O_RDONLY);
	if(fd == -1)
		perror("open file\n");

	return fd;

}

bool readline(int fd, char *data)
{
	char _ch;
	int n = 0;
	while(read(fd, &_ch, 1) == 1)
	{
		if(n >= LINE_CHAR_MAX_NUM)
		{
			printf("a line data is larger\n");
			return false;
		}
		if(_ch == '\n')
			return true;
		data[n++] = _ch;
	}

	if(n == 0)
		return false;
	else
		return true;
}

bool CloseFile(int fd)
{
	int ret = close(fd);
	if(ret == -1)
	{
		perror("close file\n");
		return false;
	}
	else
		return true;
}
