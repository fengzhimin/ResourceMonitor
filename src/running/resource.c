/******************************************************
* Author       : fengzhimin
* Create       : 2016-12-29 16:32
* Last modified: 2017-04-10 21:33
* Email        : 374648064@qq.com
* Filename     : resource.c
* Description  : 
******************************************************/
#include "running/resource.h"

static char error_info[200];
static char totalMem[2][MAX_INFOLENGTH];
static char status[FILE_PATH_MAX_LENGTH], stat[FILE_PATH_MAX_LENGTH], lineData[LINE_CHAR_MAX_NUM];
static char io[FILE_PATH_MAX_LENGTH];


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

char*** mallocResource(int oneSize, int secondSize, int thirdSize)
{
	char ***info = (char **)vmalloc(sizeof(char **)*oneSize);
	int i, j;
	for(i = 0; i < oneSize; i++)
	{
		info[i] = (char *)vmalloc(sizeof(char *)*secondSize);
		for(j = 0; j < secondSize; j++)
		{
			info[i][j] = vmalloc(sizeof(char)*thirdSize);
			memset(info[i][j], 0, thirdSize);
		}
	}

	return info;
}

void freeResource(char ***info, int oneSize, int secondSize)
{
	int i, j;
	for(i = 0; i < oneSize; i++)
	{
		for(j = 0; j < secondSize; j++)
			vfree(info[i][j]);
		vfree(info[i]);
	}
	vfree(info);
}

bool getInfoByID(char *id, char info[][MAX_INFOLENGTH], char ***allInfo, int allProcNum)
{
	int i;
	for(i = 0; i < allProcNum; i++)
	{
		//匹配当对应进程
		if(strcasecmp(allInfo[i][1], id) == 0)
		{
			int j;
			for(j = 0; j < PROCESS_INFO_NUM; j++)
			{
				memset(info[j], 0, MAX_INFOLENGTH);
				strcpy(info[j], allInfo[i][j]);
			}

			return true;
		}
	}

	return false;
}

int getProgressInfo(char ****info, char totalResouce[][MAX_INFOLENGTH])
{
	int retValue = 0;
	ProcPIDPath *beginPath = vmalloc(sizeof(ProcPIDPath));
	ProcPIDPath *currentPath = beginPath;
	int runningProcNum = getProcAll(beginPath);
	//开始截取数据包
	startHook();

	int i;
	char ***infoPre = mallocResource(runningProcNum, PROCESS_INFO_NUM, MAX_INFOLENGTH);
	//infoNext[i][0] = CPU数据
	//infoNext[i][1] = read、write系统调用次数数据
	//infoNext[i][2] = 读写磁盘的数据
	char ***infoNext = mallocResource(runningProcNum, 3, MAX_INFOLENGTH);
	for(i = 0; i < runningProcNum; i++, currentPath = currentPath->next)
	{
		memset(status, 0, FILE_PATH_MAX_LENGTH);
		memset(stat, 0, FILE_PATH_MAX_LENGTH);
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
		memset(io, 0, FILE_PATH_MAX_LENGTH);
		sprintf(status, "%s/%s", currentPath->path, "status");
		sprintf(stat, "%s/%s", currentPath->path, "stat");
		sprintf(io, "%s/%s", currentPath->path, "io");
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
		//获取进程使用CPU信息
		Process_Cpu_Occupy_t process_cpu_occupy;
		getProcessCPUTime(stat, &process_cpu_occupy);
		int process_cpu = process_cpu_occupy.utime + process_cpu_occupy.stime + process_cpu_occupy.cutime + process_cpu_occupy.cstime;
		sprintf(infoPre[i][3], "%d", process_cpu);
		//获取进程read、write系统调用信息
		Process_IO_Data processIOData;
		getProcessIOData(io, &processIOData);
		unsigned long long syscIO = processIOData.syscr + processIOData.syscw;
		unsigned long long read_write_bytes = processIOData.read_bytes + processIOData.write_bytes;
		sprintf(infoPre[i][8], "%lld", syscIO);
		sprintf(infoPre[i][9], "%lld", read_write_bytes);
	}
	Total_Cpu_Occupy_t total_cpu_occupy1;
	getTotalCPUTime(&total_cpu_occupy1);
	int total_cpu1 = total_cpu_occupy1.user + total_cpu_occupy1.nice + total_cpu_occupy1.system + total_cpu_occupy1.idle;

	//隔一段时间
	msleep(CALC_CPU_TIME);

	//停止截取数据包
	stopHook();
	PortPackageData = PortPackageData->next;  //跳过第一个元素
	while(PortPackageData != NULL)
	{
		for(i = 0, currentPath = beginPath; i < runningProcNum; i++, currentPath = currentPath->next)
		{
			if(mapProcessPort(currentPath->path, *PortPackageData))
			{
				//通过端口找到对应的进程号
				sprintf(infoPre[i][10], "%d", PortPackageData->outPackageSize);
				sprintf(infoPre[i][11], "%d", PortPackageData->inPackageSize);
				sprintf(infoPre[i][12], "%d", PortPackageData->inPackageSize + PortPackageData->outPackageSize);
			}
		}
		PortPackageData = PortPackageData->next;
	}

	for(i = 0; i < runningProcNum; i++)
	{
		memset(status, 0, FILE_PATH_MAX_LENGTH);
		memset(stat, 0, FILE_PATH_MAX_LENGTH);
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
		memset(io, 0, FILE_PATH_MAX_LENGTH);
		sprintf(status, "%s/%s", beginPath->path, "status");
		sprintf(stat, "%s/%s", beginPath->path, "stat");
		sprintf(io, "%s/%s", beginPath->path, "io");
		struct file *fp = KOpenFile(status, O_RDONLY);
		//测试文件是否存在
		if(fp == NULL)
		{
			sprintf(error_info, "%s%s%s%s%s", "打开文件: ", status, " 失败！ 错误信息： ", "    ", "\n");
			RecordLog(error_info);
			strcpy(infoNext[i][0], "processExit");
			continue;
		}
		KCloseFile(fp);
		//获取进程使用CPU信息
		Process_Cpu_Occupy_t process_cpu_occupy;
		getProcessCPUTime(stat, &process_cpu_occupy);
		int process_cpu = process_cpu_occupy.utime + process_cpu_occupy.stime + process_cpu_occupy.cutime + process_cpu_occupy.cstime;
		sprintf(infoNext[i][0], "%d", process_cpu);
		//获取进程read、write系统调用信息
		Process_IO_Data processIOData;
		getProcessIOData(io, &processIOData);
		unsigned long long syscIO = processIOData.syscr + processIOData.syscw;
		unsigned long long read_write_bytes = processIOData.read_bytes + processIOData.write_bytes;
		sprintf(infoNext[i][1], "%lld", syscIO);
		sprintf(infoNext[i][2], "%lld", read_write_bytes);
		//不断的删除path
		ProcPIDPath *temp = beginPath;
		beginPath = beginPath->next;
		vfree(temp);
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
		unsigned long long process_io1, process_io2;
		unsigned long long process_io3, process_io4;
		int pcpu;
		for(i = 0; i < runningProcNum; i++)
		{
			if(strcasecmp(infoPre[i][0], "processExit") == 0 || strcasecmp(infoNext[i][0], "processExit") == 0)
				continue;	
			vmrssNum = ExtractNumFromStr(infoPre[i][6]);
			pmem = 100*vmrssNum/totalMemNum;
			sprintf(infoPre[i][4], "%d", pmem);    //计算内存使用率
			process_cpu2 = ExtractNumFromStr(infoNext[i][0]);
			process_cpu1 = ExtractNumFromStr(infoPre[i][3]);
			pcpu = 100*(process_cpu2-process_cpu1)/(total_cpu2-total_cpu1);
			sprintf(infoPre[i][3], "%d", pcpu);
			//获取前后两次的系统调用次数
			process_io1 = ExtractNumFromStr(infoNext[i][1]);
			process_io2 = ExtractNumFromStr(infoPre[i][8]);
			//获取前后两次读写磁盘的数据量
			process_io3 = ExtractNumFromStr(infoNext[i][2]);
			process_io4 = ExtractNumFromStr(infoPre[i][9]);
			//计算一定时间时隔内系统调用的次数，用来判断对磁盘访问次数的评价
			sprintf(infoPre[i][8], "%lld", process_io1-process_io2);   
			sprintf(infoPre[i][9], "%lld", process_io3-process_io4);
			//printk("process_io3 = %lld\t process_io4 = %lld\n", process_io3, process_io4);
			retValue++;
		}
	}
	
	(*info) = mallocResource(retValue, PROCESS_INFO_NUM, MAX_INFOLENGTH);
	int temp = 0;
	for(i = 0; i < runningProcNum; i++)
	{
		if(strcasecmp(infoPre[i][0], "processExit") == 0 || strcasecmp(infoNext[i][0], "processExit") == 0)
			continue;
		int j;
		for(j = 0; j < PROCESS_INFO_NUM; j++)
		{
			strcpy((*info)[temp][j], infoPre[i][j]);
		}
		temp++;
	}
	
	freeResource(infoPre, runningProcNum, PROCESS_INFO_NUM);
	freeResource(infoNext, runningProcNum, 3);
	vfree(beginPath);
	
	return retValue;
}

