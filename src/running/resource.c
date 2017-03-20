/******************************************************
* Author       : fengzhimin
* Create       : 2016-12-29 16:32
* Last modified: 2017-03-20 20:25
* Email        : 374648064@qq.com
* Filename     : resource.c
* Description  : 
******************************************************/
#include "running/resource.h"

static char error_info[200];
static char stat_data[1000];
static char subStr18[18][MAX_SUBSTR];
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

int getProgressInfo(char path[], char info[][MAX_INFOLENGTH])
{
	memset(status, 0, FILE_PATH_MAX_LENGTH);
	memset(stat, 0, FILE_PATH_MAX_LENGTH);
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	sprintf(status, "%s/%s", path, "status");
	sprintf(stat, "%s/%s", path, "stat");
	struct file *fp = KOpenFile(status, O_RDONLY);
	if(fp == NULL)
	{
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", status, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return -1;
	}
	while(KReadLine(fp, lineData) == -1)
	{
		char subStr[2][MAX_SUBSTR];
		cutStrByLabel(lineData, ':', subStr, 2);
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
		removeChar(subStr[1], '\t');
		if(strcasecmp(subStr[0], "Name") == 0)
		{
			strcpy(info[0], subStr[1]);
			continue;
		}
		else if(strcasecmp(subStr[0], "Pid") == 0)
		{
			strcpy(info[1], subStr[1]);
			continue;
		}
		else if(strcasecmp(subStr[0], "PPid") == 0)
		{
			strcpy(info[2], subStr[1]);
			continue;
		}
		else if(strcasecmp(subStr[0], "VmPeak") == 0)
		{
			strcpy(info[5], subStr[1]);
			continue;
		}
		else if(strcasecmp(subStr[0], "VmRSS") == 0)
		{
			strcpy(info[6], subStr[1]);
			break;    //结束读取
		}
		else if(strcasecmp(subStr[0], "State") == 0)
		{
			strcpy(info[7], subStr[1]);
			continue;
		}
	}
	KCloseFile(fp);

	if(getTotalPM(totalMem) == 1)
	{
		//计算内存使用率
		unsigned int totalMemNum = ExtractNumFromStr(totalMem[0]);
		unsigned int totalFreeMem = ExtractNumFromStr(totalMem[1]);
		unsigned int vmrssNum = ExtractNumFromStr(info[6]);
		int pmem = 100*vmrssNum/totalMemNum;
		int totalpmem = 100*(totalMemNum-totalFreeMem)/totalMemNum;
		//IntToStr(info[4], pmem);
		sprintf(info[4], "%d", pmem);
		sprintf(info[9], "%d", totalpmem);
	}
	
	Total_Cpu_Occupy_t total_cpu_occupy1, total_cpu_occupy2;
	Process_Cpu_Occupy_t process_cpu_occupy1, process_cpu_occupy2;
	getTotalCPUTime(&total_cpu_occupy1);
	getProcessCPUTime(stat, &process_cpu_occupy1);
	msleep(100);
	getTotalCPUTime(&total_cpu_occupy2);
	getProcessCPUTime(stat, &process_cpu_occupy2);	
	int total_cpu1 = total_cpu_occupy1.user + total_cpu_occupy1.nice + total_cpu_occupy1.system + total_cpu_occupy1.idle;
	int total_cpu2 = total_cpu_occupy2.user + total_cpu_occupy2.nice + total_cpu_occupy2.system + total_cpu_occupy2.idle;
	int process_cpu1 = process_cpu_occupy1.utime + process_cpu_occupy1.stime + process_cpu_occupy1.cutime + process_cpu_occupy1.cstime;
	int process_cpu2 = process_cpu_occupy2.utime + process_cpu_occupy2.stime + process_cpu_occupy2.cutime + process_cpu_occupy2.cstime;
	int pcpu = 100*(process_cpu2-process_cpu1)/(total_cpu2-total_cpu1);
	//IntToStr(info[3], pcpu);
	sprintf(info[3], "%d", pcpu);
	int totalcpu = total_cpu2 - total_cpu1;
	int totalidle = total_cpu_occupy2.idle - total_cpu_occupy1.idle;
	int totalpcpu = 100*(totalcpu-totalidle)/totalcpu;
	sprintf(info[8], "%d", totalpcpu);
	return 1;
}

int getTotalPM(char totalMem[][MAX_INFOLENGTH])
{
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	int lineNum = 1;
	struct file *fp = KOpenFile("/proc/meminfo", O_RDONLY);
	char subStr[2][MAX_SUBSTR];
	if(fp == NULL)
	{
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", "/proc/meminfo", " 失败！ 错误信息： ", "   ", "\n");
		RecordLog(error_info);
		return -1;
	}
	while(KReadLine(fp, lineData) == -1)
	{
		if(lineNum == 1)
		{
			//提取/proc/meminfo 中的第一行数据(MemTotal)
			cutStrByLabel(lineData, ':', subStr, 2);
			removeChar(subStr[1], '\t');
			strcpy(totalMem[0], subStr[1]);
		}
		else if(lineNum == 3)
		{
			//提取/proc/meminfo 中的第三行数据(MemAvailable)
			cutStrByLabel(lineData, ':', subStr, 2);
			removeChar(subStr[1], '\t');
			strcpy(totalMem[1], subStr[1]);
			break;
		}
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
		lineNum++;
	}
	if(lineNum == 1)
	{
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", "/proc/meminfo", " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		KCloseFile(fp);
		return -1;
	}
	KCloseFile(fp);
	return 1;
}

int getProcessCPUTime(char *stat, Process_Cpu_Occupy_t *processCpuTime)
{
	struct file *fp = KOpenFile(stat, O_RDONLY);
	if(fp == NULL)
	{
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", stat, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return -1;
	}

	memset(stat_data, 0, 1000);
	//if(KReadLine(fp, stat_data) == -1)
	int size = KReadFile(fp, stat_data, 1000);
	if(size > 0)
	{
		//printk("size = %d\n", size);
		cutStrByLabel(stat_data, ' ', subStr18, 18);
		processCpuTime->pid = ExtractNumFromStr(subStr18[12]);
		processCpuTime->utime = ExtractNumFromStr(subStr18[13]);
		processCpuTime->stime = ExtractNumFromStr(subStr18[14]);
		processCpuTime->cutime = ExtractNumFromStr(subStr18[15]);
		processCpuTime->cstime = ExtractNumFromStr(subStr18[16]);
		KCloseFile(fp);
		return 1;
	}
	else
	{
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", stat, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		KCloseFile(fp);
		return -1;
	}
}

int getTotalCPUTime(Total_Cpu_Occupy_t *totalCpuTime)
{
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	struct file *fp = KOpenFile("/proc/stat", O_RDONLY);
	if(fp == NULL)
	{
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", "/proc/stat", " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return -1;
	}
	if(KReadLine(fp, lineData) == -1)
	{
		char name[30];
		sscanf(lineData, "%s %u %u %u %u", name, &totalCpuTime->user, &totalCpuTime->nice, &totalCpuTime->system, &totalCpuTime->idle);
		KCloseFile(fp);
		return 1;
	}
	else
	{
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", "/proc/stat", " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		KCloseFile(fp);
		return -1;
	}
}
