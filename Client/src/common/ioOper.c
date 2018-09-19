/******************************************************
* Author       : fengzhimin
* Create       : 2018-09-18 19:47
* Last modified: 2018-09-18 19:47
* Email        : 374648064@qq.com
* Filename     : ioOper.c
* Description  : common io operation
******************************************************/

#include <common/ioOper.h>

char getch()
{
	fd_set rfds;
	struct timeval tv;
	char ch = 0;
	FD_ZERO(&rfds);
	FD_SET(STDIN_FILENO, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 10;

	if(select(1, &rfds, NULL, NULL, &tv) > 0)
	{
		ch = getchar();
	}

	return ch;
}
