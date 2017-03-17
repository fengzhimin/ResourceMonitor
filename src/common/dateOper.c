/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-04 12:49
* Last modified: 2016-11-05 01:21
* Email        : 374648064@qq.com
* Filename     : dateOper.c
* Description  : 
******************************************************/

#include "common/dateOper.h"

int GetLocalTime(char *buf, int fmt)
{
	if(fmt < 0 || fmt > 2)
		return -1;
    time_t t;
	struct tm *tp;
	t = time(NULL);
	 
	tp = localtime(&t);
	switch(fmt)
	{
	case 0:
		sprintf(buf, "%2.2d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", tp->tm_year+1900, tp->tm_mon+1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
		break;
	case 1:
		sprintf(buf, "%2.2d-%2.2d-%2.2d", tp->tm_year+1900, tp->tm_mon+1, tp->tm_mday);
		break;
	case 2:
		sprintf(buf, "%2.2d:%2.2d:%2.2d", tp->tm_hour, tp->tm_min, tp->tm_sec);
		break;
	default:
		break;
	}

	return 0;
}
