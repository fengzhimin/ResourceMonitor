/******************************************************
* Author       : fengzhimin
* Create       : 2016-12-29 16:32
* Last modified: 2017-04-04 14:14
* Email        : 374648064@qq.com
* Filename     : resource.c
* Description  : 
******************************************************/
#include "running/resource.h"

static char error_info[200];
static char totalMem[2][MAX_INFOLENGTH];
static char status[FILE_PATH_MAX_LENGTH], stat[FILE_PATH_MAX_LENGTH], lineData[LINE_CHAR_MAX_NUM];


/*
int getStatusPathByName(char name[], char path[])
{
	char *root_path = "/proc";
	DIR *pdir;
	struct dirent *pdirent;
	pdir = opendir(root_path);
	if(pdir == NULL)
	{
		sprintf(error_info, "%s%s%s%s%s", "打开文件夹: ", root_path, " 失败！ 错误信息： ", strerror(errno), "\n");
		RecordLog(error_info);
		return -1;
	}
	while((pdirent = readdir(pdir)) != NULL)
	{
		if(strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0)
			continue;
		char temp[FILE_PATH_MAX_LENGTH], temp1[LINE_CHAR_MAX_NUM], temp2[FILE_PATH_MAX_LENGTH];
		memset(temp, 0, FILE_PATH_MAX_LENGTH);
		memset(temp2, 0, FILE_PATH_MAX_LENGTH);
		memset(temp1, 0, LINE_CHAR_MAX_NUM);
		sprintf(temp, "%s/%s", root_path, pdirent->d_name);
		if(Is_Dir(temp) == -2)   //判断是否为普通文件
			continue;
		else if(Is_Dir(temp) == -1)
		{
			sprintf(temp2, "%s/%s", temp, "status");
			if(access(temp2, F_OK) != -1)   //判断目录下的status文件是否存在
			{
				FILE *fd = OpenFile(temp2, "r");
				if(fd == NULL)
				{
					char error_info[200];
					sprintf(error_info, "%s%s%s%s%s", "打开文件: ", temp, " 失败！ 错误信息： ", strerror(errno), "\n");
					RecordLog(error_info);
					return -1;
				}
				if(ReadLine(fd, temp1) == -1)
				{
					char subStr[2][MAX_SUBSTR];
					cutStrByLabel(temp1, ':', subStr, 2);
					removeChar(subStr[1], '\t');
					if(strcasecmp(subStr[1], name) == 0)
					{
						strcpy(path, temp);
						CloseFile(fd);
						return 1;
					}
				}
				else
				{
					char error_info[200];
					sprintf(error_info, "%s%s%s%s%s", "读取文件: ", temp, " 失败！ 错误信息： ", strerror(errno), "\n");
					RecordLog(error_info);
				}

				CloseFile(fd);
			}
		}
	}

	return 0;
}
*/

int getProcAll(ProcPIDPath *path)
{
	struct task_struct *task, *p;
	struct list_head *ps;
	int count = 0;
	task = &init_task;
	ProcPIDPath *temp = path;
	list_for_each(ps, &task->tasks)
	{
		p = list_entry(ps, struct task_struct, tasks);
		memset(temp->path, 0, MAX_PROCPATH);
		sprintf(temp->path, "%s/%d", "/proc", p->pid);
		temp = temp->next = vmalloc(sizeof(ProcPIDPath));
		count++;
	}

	return count;
}

int getProgressInfo(char ***info, char totalResouce[][MAX_INFOLENGTH])
{
	int retValue = 0;
	ProcPIDPath *path = vmalloc(sizeof(ProcPIDPath));
	ProcPIDPath *path1 = path;
	int runningProcNum = getProcAll(path);
	//char (*infoPre)[PROCESS_INFO_NUM][MAX_INFOLENGTH] = vmalloc(sizeof(char)*runningProcNum);
	//char (*infoNext)[MAX_INFOLENGTH] = vmalloc(sizeof(char)*runningProcNum);  //存放一段时间后的CPU时间
	int i;
	char ***infoPre = (char **)vmalloc(sizeof(char **)*runningProcNum);
	for(i = 0; i < runningProcNum; i++)
	{
		infoPre[i] = (char *)vmalloc(sizeof(char *)*PROCESS_INFO_NUM);
		int j;
		for(j = 0; j < PROCESS_INFO_NUM; j++)
		{
			infoPre[i][j] = vmalloc(sizeof(char)*MAX_INFOLENGTH);
			memset(infoPre[i][j], 0, MAX_INFOLENGTH);
		}
	}
	char **infoNext = (char *)vmalloc(sizeof(char *)*runningProcNum);
	for(i = 0; i < runningProcNum; i++)
	{
		infoNext[i] = vmalloc(sizeof(char)*MAX_INFOLENGTH);
	}
	for(i = 0; i < runningProcNum; i++, path = path->next)
	{
		memset(status, 0, FILE_PATH_MAX_LENGTH);
		memset(stat, 0, FILE_PATH_MAX_LENGTH);
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
		sprintf(status, "%s/%s", path->path, "status");
		sprintf(stat, "%s/%s", path->path, "stat");
		struct file *fp = KOpenFile(status, O_RDONLY);
		if(fp == NULL)
		{
			sprintf(error_info, "%s%s%s%s%s", "打开文件: ", status, " 失败！ 错误信息： ", "    ", "\n");
			RecordLog(error_info);
			strcpy(infoPre[i][0], "processExit");
			continue;
		}
		while(KReadLine(fp, lineData) == -1)
		{
			char subStr[2][MAX_SUBSTR];
			cutStrByLabel(lineData, ':', subStr, 2);
			memset(lineData, 0, LINE_CHAR_MAX_NUM);
			removeChar(subStr[1], '\t');
			if(strcasecmp(subStr[0], "Name") == 0)
			{
				strcpy(infoPre[i][0], subStr[1]);
				continue;
			}
			else if(strcasecmp(subStr[0], "Pid") == 0)
			{
				strcpy(infoPre[i][1], subStr[1]);
				continue;
			}
			else if(strcasecmp(subStr[0], "PPid") == 0)
			{
				strcpy(infoPre[i][2], subStr[1]);
				continue;
			}
			else if(strcasecmp(subStr[0], "VmPeak") == 0)
			{
				strcpy(infoPre[i][5], subStr[1]);
				continue;
			}
			else if(strcasecmp(subStr[0], "VmRSS") == 0)
			{
				strcpy(infoPre[i][6], subStr[1]);
				break;    //结束读取
			}
			else if(strcasecmp(subStr[0], "State") == 0)
			{
				strcpy(infoPre[i][7], subStr[1]);
				continue;
			}
		}
		KCloseFile(fp);
		Process_Cpu_Occupy_t process_cpu_occupy;
		getProcessCPUTime(stat, &process_cpu_occupy);
		int process_cpu = process_cpu_occupy.utime + process_cpu_occupy.stime + process_cpu_occupy.cutime + process_cpu_occupy.cstime;
		sprintf(infoPre[i][3], "%d", process_cpu);
	}
	Total_Cpu_Occupy_t total_cpu_occupy1;
	getTotalCPUTime(&total_cpu_occupy1);
	int total_cpu1 = total_cpu_occupy1.user + total_cpu_occupy1.nice + total_cpu_occupy1.system + total_cpu_occupy1.idle;
	msleep(50);
	for(i = 0; i < runningProcNum; i++)
	{
		memset(status, 0, FILE_PATH_MAX_LENGTH);
		memset(stat, 0, FILE_PATH_MAX_LENGTH);
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
		sprintf(status, "%s/%s", path1->path, "status");
		sprintf(stat, "%s/%s", path1->path, "stat");
		struct file *fp = KOpenFile(status, O_RDONLY);
		if(fp == NULL)
		{
			sprintf(error_info, "%s%s%s%s%s", "打开文件: ", status, " 失败！ 错误信息： ", "    ", "\n");
			RecordLog(error_info);
			strcpy(infoNext[i], "processExit");
			continue;
		}
		KCloseFile(fp);
		Process_Cpu_Occupy_t process_cpu_occupy;
		getProcessCPUTime(stat, &process_cpu_occupy);
		int process_cpu = process_cpu_occupy.utime + process_cpu_occupy.stime + process_cpu_occupy.cutime + process_cpu_occupy.cstime;
		sprintf(infoNext[i], "%d", process_cpu);
		ProcPIDPath *temp = path1->next;
		vfree(path1);
		path1 = temp;
	}
	Total_Cpu_Occupy_t total_cpu_occupy2;
	getTotalCPUTime(&total_cpu_occupy2);
	int total_cpu2 = total_cpu_occupy2.user + total_cpu_occupy2.nice + total_cpu_occupy2.system + total_cpu_occupy2.idle;	
	//计算总CPU使用率
	int totalcpu = total_cpu2 - total_cpu1;
	int totalidle = total_cpu_occupy2.idle - total_cpu_occupy1.idle;
	int totalpcpu = 100*(totalcpu-totalidle)/totalcpu;
	sprintf(totalResouce[0], "%d", totalpcpu);
	if(getTotalPM(totalMem) == 1)
	{
		//计算内存使用率
		unsigned int totalMemNum = ExtractNumFromStr(totalMem[0]);
		unsigned int totalFreeMem = ExtractNumFromStr(totalMem[1]);
		int totalpmem = 100*(totalMemNum-totalFreeMem)/totalMemNum;
		sprintf(totalResouce[1], "%d", totalpmem);
		unsigned int vmrssNum;
		int pmem;
		int process_cpu1, process_cpu2;
		int pcpu;
		for(i = 0; i < runningProcNum; i++)
		{
			if(strcasecmp(infoPre[i][0], "processExit") == 0 || strcasecmp(infoNext[i], "processExit") == 0)
				continue;	
			vmrssNum = ExtractNumFromStr(infoPre[i][6]);
			pmem = 100*vmrssNum/totalMemNum;
			sprintf(infoPre[i][4], "%d", pmem);    //计算内存使用率
			process_cpu2 = ExtractNumFromStr(infoNext[i]);
			process_cpu1 = ExtractNumFromStr(infoPre[i][3]);
			pcpu = 100*(process_cpu2-process_cpu1)/(total_cpu2-total_cpu1);
			sprintf(infoPre[i][3], "%d", pcpu);
			retValue++;
		}
	}

	info = (char **)vmalloc(sizeof(char **)*retValue);
	for(i = 0; i < retValue; i++)
	{
		info[i] = (char *)vmalloc(sizeof(char *)*PROCESS_INFO_NUM);
		int j;
		for(j = 0; j < PROCESS_INFO_NUM; j++)
		{
			info[i][j] = vmalloc(sizeof(char)*MAX_INFOLENGTH);
			memset(info[i][j], 0, MAX_INFOLENGTH);
		}
	}
	int temp = 0;
	for(i = 0; i < runningProcNum; i++)
	{
		if(strcasecmp(infoPre[i][0], "processExit") == 0 || strcasecmp(infoNext[i], "processExit") == 0)
			continue;
		int j;
		for(j = 0; j < PROCESS_INFO_NUM; j++)
		{
			strcpy(info[temp][j], infoPre[i][j]);
		}
		temp++;
	}

	for(i = 0; i < runningProcNum; i++)
	{
		int j;
		for(j = 0; j < PROCESS_INFO_NUM; j++)
			vfree(infoPre[i][j]);
		vfree(infoPre[i]);
	}
	for(i = 0; i < runningProcNum; i++)
		vfree(infoNext[i]);
	vfree(infoPre);
	vfree(infoNext);
	vfree(path1);
	
	return retValue;
}

