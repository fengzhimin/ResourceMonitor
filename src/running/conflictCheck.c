/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-11 10:02
* Last modified: 2017-04-11 10:02
* Email        : 374648064@qq.com
* Filename     : conflictCheck.c
* Description  : 
******************************************************/

#include "running/conflictCheck.h"

void solveProcessRelate(char ***info, int processNum)
{
	int i, temp;
	bool ret;
	char parentInfo[PROCESS_INFO_NUM][MAX_INFOLENGTH];
	for(i = processNum-1; i >= 0; i--)
	{
		temp = StrToInt(info[i][3]);
		//当进程的CPU使用率符合需求时,查找其对应的父进程和父父进程的资源使用情况
		if(temp >= PROCESSRELATECPUDOWN && temp <= PROCESSRELATECPUUP)
		{
			//获取其父进程的资源使用情况
			int temp_cpu, temp_mem;
			ret = getInfoByID(info[i][2], parentInfo, info, processNum);
			if(ret)
			{
				temp_cpu = StrToInt(info[i][3]) + StrToInt(parentInfo[3]);
				temp_mem = StrToInt(info[i][4]) + StrToInt(parentInfo[4]);
				memset(info[i][3], 0, MAX_INFOLENGTH);
				memset(info[i][4], 0, MAX_INFOLENGTH);
				IntToStr(info[i][3], temp_cpu);
				IntToStr(info[i][4], temp_mem);
				//获取父父进程
				ret = getInfoByID(parentInfo[2], parentInfo, info, processNum);
				if(ret)
				{
					temp_cpu = StrToInt(info[i][3]) + StrToInt(parentInfo[3]);
					temp_mem = StrToInt(info[i][4]) + StrToInt(parentInfo[4]);
					memset(info[i][3], 0, MAX_INFOLENGTH);
					memset(info[i][4], 0, MAX_INFOLENGTH);
					IntToStr(info[i][3], temp_cpu);
					IntToStr(info[i][4], temp_mem);
				}
			}
		}
	}
}
