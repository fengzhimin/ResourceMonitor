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
static char status[MAX_PROCPATH], stat[MAX_PROCPATH], lineData[LINE_CHAR_MAX_NUM];
static char io[MAX_PROCPATH];
static char sched[MAX_PROCPATH];


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

/*
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
*/

bool getInfoByID(int id, ProcInfo *info, ProcInfo allInfo[], int allProcNum)
{
	int i;
	for(i = 0; i < allProcNum; i++)
	{
		//匹配当对应进程
		if(allInfo[i].pid == id)
		{
			*info = allInfo[i];
			return true;
		}
	}

	return false;
}

int getProgressInfo(ProcInfo **info, SysResource *totalResource)
{
	int retValue = 0;
	ProcPIDPath *beginPath = vmalloc(sizeof(ProcPIDPath));
	ProcPIDPath *currentPath = beginPath;
	int runningProcNum = getProcAll(beginPath);
	//开始截取数据包
	startHook();

	int i;
	ProcInfo *infoPre = vmalloc(sizeof(ProcInfo)*runningProcNum);
	for(i = 0; i < runningProcNum; i++)
		memset(&infoPre[i], 0, sizeof(ProcInfo));

	ProcInfo *infoNext = vmalloc(sizeof(ProcInfo)*runningProcNum);
	for(i = 0; i < runningProcNum; i++)
		memset(&infoNext[i], 0, sizeof(ProcInfo));

	for(i = 0; i < runningProcNum; i++, currentPath = currentPath->next)
	{
		memset(status, 0, MAX_PROCPATH);
		memset(stat, 0, MAX_PROCPATH);
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
		memset(io, 0, MAX_PROCPATH);
		memset(sched, 0, MAX_PROCPATH);
		sprintf(status, "%s/%s", currentPath->path, "status");
		sprintf(stat, "%s/%s", currentPath->path, "stat");
		sprintf(io, "%s/%s", currentPath->path, "io");
		sprintf(sched, "%s/%s", currentPath->path, "sched");
		struct file *fp = KOpenFile(status, O_RDONLY);
		if(fp == NULL)
		{
			sprintf(error_info, "%s%s%s%s%s", "打开文件: ", status, " 失败！ 错误信息： ", "    ", "\n");
			RecordLog(error_info);
			strcpy(infoPre[i].name, "processExit");
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
				strcpy(infoPre[i].name, subStr[1]);
				continue;
			}
			else if(strcasecmp(subStr[0], "Pid") == 0)
			{
				infoPre[i].pid = ExtractNumFromStr(subStr[1]);
				continue;
			}
			else if(strcasecmp(subStr[0], "PPid") == 0)
			{
				infoPre[i].ppid = ExtractNumFromStr(subStr[1]);
				continue;
			}
			else if(strcasecmp(subStr[0], "VmPeak") == 0)
			{
				strcpy(infoPre[i].VmPeak, subStr[1]);
				continue;
			}
			else if(strcasecmp(subStr[0], "VmRSS") == 0)
			{
				strcpy(infoPre[i].VmRSS, subStr[1]);
				break;    //结束读取
			}
			else if(strcasecmp(subStr[0], "State") == 0)
			{
				strcpy(infoPre[i].State, subStr[1]);
				continue;
			}
		}
		KCloseFile(fp);
		//获取进程使用CPU信息
		Process_Cpu_Occupy_t process_cpu_occupy;
		getProcessCPUTime(stat, &process_cpu_occupy);
		infoPre[i].cpuUsed = process_cpu_occupy.utime + process_cpu_occupy.stime + process_cpu_occupy.cutime + process_cpu_occupy.cstime;
		//获取进程read、write系统调用信息
		Process_IO_Data processIOData;
		getProcessIOData(io, &processIOData);
		infoPre[i].ioSyscallNum = processIOData.syscr + processIOData.syscw;
		infoPre[i].ioDataBytes = processIOData.read_bytes + processIOData.write_bytes;
		//获取进程的sched
		infoPre[i].schedInfo = getProcSchedInfo(sched);
	}
	Total_Cpu_Occupy_t total_cpu_occupy1;
	getTotalCPUTime(&total_cpu_occupy1);
	int total_cpu1 = total_cpu_occupy1.user + total_cpu_occupy1.nice + total_cpu_occupy1.system + total_cpu_occupy1.idle;
	//获取系统的网络使用情况
	NetInfo *totalNet1;
	int totalNetInfoNum1 = getTotalNet(&totalNet1);
	DiskInfo *totalDiskInfo1;
	int totalDiskInfoNum1 = getAllDiskState(&totalDiskInfo1);

	//隔一段时间
	msleep(CALC_CPU_TIME);

	DiskInfo *totalDiskInfo2;
	int totalDiskInfoNum2 = getAllDiskState(&totalDiskInfo2);
	DiskInfo *curDiskInfo1 = totalDiskInfo1;
	DiskInfo *curDiskInfo2 = totalDiskInfo2;
	if(totalDiskInfoNum1 == totalDiskInfoNum2 && totalDiskInfoNum1 != 0)
	{
		int handle_IO_time = 0;
		IOUsedInfo *tailIOUsedInfo;
		totalResource->ioUsed = tailIOUsedInfo = NULL;
		while(curDiskInfo1 != NULL)
		{
			handle_IO_time = (curDiskInfo2->diskInfo.ticks - curDiskInfo1->diskInfo.ticks);
			//计算每个磁盘的使用率
			if(tailIOUsedInfo == NULL)
			{
				totalResource->ioUsed = tailIOUsedInfo = vmalloc(sizeof(IOUsedInfo));
			}
			else
			{
				tailIOUsedInfo = tailIOUsedInfo->next = vmalloc(sizeof(IOUsedInfo));
			}
			strcpy(tailIOUsedInfo->diskName, curDiskInfo1->diskName);
			tailIOUsedInfo->ioUsed = handle_IO_time*100/CALC_CPU_TIME;
			tailIOUsedInfo->next = NULL;

			curDiskInfo1 = curDiskInfo1->next;
			curDiskInfo2 = curDiskInfo2->next;
		}
		//释放列表资源
		while(totalDiskInfo1 != NULL)
		{
			curDiskInfo1 = totalDiskInfo1;
			curDiskInfo2 = totalDiskInfo2;
			totalDiskInfo1 = totalDiskInfo1->next;
			totalDiskInfo2 = totalDiskInfo2->next;
			vfree(curDiskInfo1);
			vfree(curDiskInfo2);
		}
	}
	else
	{
		//针对前后两次磁盘的个数不一致的情况，直接忽略这次检测
		totalResource->ioUsed = NULL;
		//释放列表资源
		while(totalDiskInfo1 != NULL)
		{
			curDiskInfo1 = totalDiskInfo1;
			totalDiskInfo1 = totalDiskInfo1->next;
			vfree(curDiskInfo1);
		}
		while(totalDiskInfo2 != NULL)
		{
			curDiskInfo2 = totalDiskInfo2;
			totalDiskInfo2 = totalDiskInfo2->next;
			vfree(curDiskInfo2);
		}
	}

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
				infoPre[i].uploadPackage = PortPackageData->outPackageSize;
				infoPre[i].downloadPackage = PortPackageData->inPackageSize;
				infoPre[i].totalPackage = infoPre[i].uploadPackage + infoPre[i].downloadPackage;
				infoPre[i].uploadBytes = PortPackageData->outDataBytes;
				infoPre[i].downloadBytes = PortPackageData->inDataBytes;
				infoPre[i].totalBytes = infoPre[i].uploadBytes + infoPre[i].downloadBytes;
			}
		}
		PortPackageData = PortPackageData->next;
	}

	for(i = 0; i < runningProcNum; i++)
	{
		memset(status, 0, MAX_PROCPATH);
		memset(stat, 0, MAX_PROCPATH);
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
		memset(io, 0, MAX_PROCPATH);
		memset(sched, 0, MAX_PROCPATH);
		sprintf(status, "%s/%s", beginPath->path, "status");
		sprintf(stat, "%s/%s", beginPath->path, "stat");
		sprintf(io, "%s/%s", beginPath->path, "io");
		sprintf(sched, "%s/%s", beginPath->path, "sched");
		struct file *fp = KOpenFile(status, O_RDONLY);
		//测试文件是否存在
		if(fp == NULL)
		{
			sprintf(error_info, "%s%s%s%s%s", "打开文件: ", status, " 失败！ 错误信息： ", "    ", "\n");
			RecordLog(error_info);
			strcpy(infoNext[i].name, "processExit");
			continue;
		}
		KCloseFile(fp);
		//获取进程使用CPU信息
		Process_Cpu_Occupy_t process_cpu_occupy;
		getProcessCPUTime(stat, &process_cpu_occupy);
		infoNext[i].cpuUsed = process_cpu_occupy.utime + process_cpu_occupy.stime + process_cpu_occupy.cutime + process_cpu_occupy.cstime;
		//获取进程read、write系统调用信息
		Process_IO_Data processIOData;
		getProcessIOData(io, &processIOData);
		infoNext[i].ioSyscallNum = processIOData.syscr + processIOData.syscw;
		infoNext[i].ioDataBytes = processIOData.read_bytes + processIOData.write_bytes;
		//获取进程的sched
		infoNext[i].schedInfo = getProcSchedInfo(sched);
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
	totalResource->cpuUsed = 100*(totalcpu-totalidle)/totalcpu;
	MemInfo totalMem;
	if(getTotalPM(&totalMem))
	{
		//计算内存使用率
		totalResource->memUsed = 100*(totalMem.memTotal-totalMem.memAvailable)/totalMem.memTotal;
		unsigned int vmrssNum;
		for(i = 0; i < runningProcNum; i++)
		{
			if(strcasecmp(infoPre[i].name, "processExit") == 0 || strcasecmp(infoNext[i].name, "processExit") == 0)
				continue;	
			vmrssNum = ExtractNumFromStr(infoPre[i].VmRSS);
			infoPre[i].memUsed = 100*vmrssNum/totalMem.memTotal;
			infoPre[i].cpuUsed = 100*(infoNext[i].cpuUsed-infoPre[i].cpuUsed)/(total_cpu2-total_cpu1);
			//计算一定时间间隔内系统调用的次数，用来判断对磁盘访问次数的评价
			infoPre[i].ioSyscallNum = infoNext[i].ioSyscallNum - infoPre[i].ioSyscallNum;
			infoPre[i].ioDataBytes = infoNext[i].ioDataBytes - infoPre[i].ioDataBytes;
			//计算一定时间间隔内进程sched
			infoPre[i].schedInfo.sum_exec_runtime = infoNext[i].schedInfo.sum_exec_runtime - infoPre[i].schedInfo.sum_exec_runtime;
			infoPre[i].schedInfo.wait_sum = infoNext[i].schedInfo.wait_sum - infoPre[i].schedInfo.wait_sum;
			infoPre[i].schedInfo.iowait_sum = infoNext[i].schedInfo.iowait_sum - infoPre[i].schedInfo.iowait_sum;
			retValue++;
		}
	}

	//获取系统的网络实时情况
	NetInfo *totalNet2;
	int totalNetInfoNum2 = getTotalNet(&totalNet2);
	NetInfo *curNetInfo1 = totalNet1;
	NetInfo *curNetInfo2 = totalNet2;
	if(totalNetInfoNum1 == totalNetInfoNum2 && totalNetInfoNum1 != 0)
	{
		NetUsedInfo *tailNetUsedInfo;
		totalResource->netUsed = tailNetUsedInfo = NULL;
		while(curNetInfo1 != NULL)
		{
			//计算每个网卡的使用率
			if(tailNetUsedInfo == NULL)
			{
				totalResource->netUsed = tailNetUsedInfo = vmalloc(sizeof(NetUsedInfo));
			}
			else
			{
				tailNetUsedInfo = tailNetUsedInfo->next = vmalloc(sizeof(NetUsedInfo));
			}
			strcpy(tailNetUsedInfo->netCardName, curNetInfo1->netCardInfo.netCardName);
			tailNetUsedInfo->next = NULL;
			tailNetUsedInfo->uploadPackage = curNetInfo2->netCardInfo.uploadPackage - curNetInfo1->netCardInfo.uploadPackage;
			tailNetUsedInfo->downloadPackage = curNetInfo2->netCardInfo.downloadPackage - curNetInfo1->netCardInfo.downloadPackage;
			tailNetUsedInfo->totalPackage = tailNetUsedInfo->uploadPackage + tailNetUsedInfo->downloadPackage;
			tailNetUsedInfo->uploadBytes = curNetInfo2->netCardInfo.uploadBytes - curNetInfo1->netCardInfo.uploadBytes;
			tailNetUsedInfo->downloadBytes = curNetInfo2->netCardInfo.downloadBytes - curNetInfo1->netCardInfo.downloadBytes;
			tailNetUsedInfo->totalBytes = tailNetUsedInfo->uploadBytes + tailNetUsedInfo->downloadBytes;

			curNetInfo1 = curNetInfo1->next;
			curNetInfo2 = curNetInfo2->next;
		}
		//释放列表资源
		while(totalNet1 != NULL)
		{
			curNetInfo1 = totalNet1;
			curNetInfo2 = totalNet2;
			totalNet1 = totalNet1->next;
			totalNet2 = totalNet2->next;
			vfree(curNetInfo1);
			vfree(curNetInfo2);
		}
	}
	else
	{
		//当前后两次网卡数量不一致的时，直接忽略
		totalResource->netUsed = NULL;
		//释放列表资源
		while(totalNet1 != NULL)
		{
			curNetInfo1 = totalNet1;
			totalNet1 = totalNet1->next;
			vfree(curNetInfo1);
		}
		while(totalNet2 != NULL)
		{
			curNetInfo2 = totalNet2;
			totalNet2 = totalNet2->next;
			vfree(curNetInfo2);
		}
	}
	
	(*info) = vmalloc(sizeof(ProcInfo)*retValue);
	int temp = 0;
	for(i = 0; i < runningProcNum; i++)
	{
		if(strcasecmp(infoPre[i].name, "processExit") == 0 || strcasecmp(infoNext[i].name, "processExit") == 0)
			continue;
		(*info)[temp++] = infoPre[i];
	}
	
	vfree(infoPre);
	vfree(infoNext);
	vfree(beginPath);
	
	return retValue;
}

void getSysResourceInfo(SysResource *totalResource)
{
	Total_Cpu_Occupy_t total_cpu_occupy1;
	getTotalCPUTime(&total_cpu_occupy1);
	int total_cpu1 = total_cpu_occupy1.user + total_cpu_occupy1.nice + total_cpu_occupy1.system + total_cpu_occupy1.idle;
	//获取系统的网络使用情况
	NetInfo *totalNet1;
	int totalNetInfoNum1 = getTotalNet(&totalNet1);
	DiskInfo *totalDiskInfo1;
	int totalDiskInfoNum1 = getAllDiskState(&totalDiskInfo1);

	//隔一段时间
	msleep(CALC_CPU_TIME);

	DiskInfo *totalDiskInfo2;
	int totalDiskInfoNum2 = getAllDiskState(&totalDiskInfo2);
	DiskInfo *curDiskInfo1 = totalDiskInfo1;
	DiskInfo *curDiskInfo2 = totalDiskInfo2;
	if(totalDiskInfoNum1 == totalDiskInfoNum2 && totalDiskInfoNum1 != 0)
	{
		int handle_IO_time = 0;
		IOUsedInfo *tailIOUsedInfo;
		totalResource->ioUsed = tailIOUsedInfo = NULL;
		while(curDiskInfo1 != NULL)
		{
			handle_IO_time = (curDiskInfo2->diskInfo.ticks - curDiskInfo1->diskInfo.ticks);
			//计算每个磁盘的使用率
			if(tailIOUsedInfo == NULL)
			{
				totalResource->ioUsed = tailIOUsedInfo = vmalloc(sizeof(IOUsedInfo));
			}
			else
			{
				tailIOUsedInfo = tailIOUsedInfo->next = vmalloc(sizeof(IOUsedInfo));
			}
			strcpy(tailIOUsedInfo->diskName, curDiskInfo1->diskName);
			tailIOUsedInfo->ioUsed = handle_IO_time*100/CALC_CPU_TIME;
			tailIOUsedInfo->next = NULL;

			curDiskInfo1 = curDiskInfo1->next;
			curDiskInfo2 = curDiskInfo2->next;
		}
		//释放列表资源
		while(totalDiskInfo1 != NULL)
		{
			curDiskInfo1 = totalDiskInfo1;
			curDiskInfo2 = totalDiskInfo2;
			totalDiskInfo1 = totalDiskInfo1->next;
			totalDiskInfo2 = totalDiskInfo2->next;
			vfree(curDiskInfo1);
			vfree(curDiskInfo2);
		}
	}
	else
	{
		//针对前后两次磁盘的个数不一致的情况，直接忽略这次检测
		totalResource->ioUsed = NULL;
		//释放列表资源
		while(totalDiskInfo1 != NULL)
		{
			curDiskInfo1 = totalDiskInfo1;
			totalDiskInfo1 = totalDiskInfo1->next;
			vfree(curDiskInfo1);
		}
		while(totalDiskInfo2 != NULL)
		{
			curDiskInfo2 = totalDiskInfo2;
			totalDiskInfo2 = totalDiskInfo2->next;
			vfree(curDiskInfo2);
		}
	}
	Total_Cpu_Occupy_t total_cpu_occupy2;
	getTotalCPUTime(&total_cpu_occupy2);
	int total_cpu2 = total_cpu_occupy2.user + total_cpu_occupy2.nice + total_cpu_occupy2.system + total_cpu_occupy2.idle;	
	//计算总CPU使用率
	int totalcpu = total_cpu2 - total_cpu1;
	int totalidle = total_cpu_occupy2.idle - total_cpu_occupy1.idle;
	totalResource->cpuUsed = 100*(totalcpu-totalidle)/totalcpu;
	MemInfo totalMem;
	if(getTotalPM(&totalMem))
	{
		//计算内存使用率
		totalResource->memUsed = 100*(totalMem.memTotal-totalMem.memAvailable)/totalMem.memTotal;
	}
	//获取系统的网络实时情况
	NetInfo *totalNet2;
	int totalNetInfoNum2 = getTotalNet(&totalNet2);
	NetInfo *curNetInfo1 = totalNet1;
	NetInfo *curNetInfo2 = totalNet2;
	if(totalNetInfoNum1 == totalNetInfoNum2 && totalNetInfoNum1 != 0)
	{
		NetUsedInfo *tailNetUsedInfo;
		totalResource->netUsed = tailNetUsedInfo = NULL;
		while(curNetInfo1 != NULL)
		{
			//计算每个网卡的使用率
			if(tailNetUsedInfo == NULL)
			{
				totalResource->netUsed = tailNetUsedInfo = vmalloc(sizeof(NetUsedInfo));
			}
			else
			{
				tailNetUsedInfo = tailNetUsedInfo->next = vmalloc(sizeof(NetUsedInfo));
			}
			strcpy(tailNetUsedInfo->netCardName, curNetInfo1->netCardInfo.netCardName);
			tailNetUsedInfo->next = NULL;
			tailNetUsedInfo->uploadPackage = curNetInfo2->netCardInfo.uploadPackage - curNetInfo1->netCardInfo.uploadPackage;
			tailNetUsedInfo->downloadPackage = curNetInfo2->netCardInfo.downloadPackage - curNetInfo1->netCardInfo.downloadPackage;
			tailNetUsedInfo->totalPackage = tailNetUsedInfo->uploadPackage + tailNetUsedInfo->downloadPackage;
			tailNetUsedInfo->uploadBytes = curNetInfo2->netCardInfo.uploadBytes - curNetInfo1->netCardInfo.uploadBytes;
			tailNetUsedInfo->downloadBytes = curNetInfo2->netCardInfo.downloadBytes - curNetInfo1->netCardInfo.downloadBytes;
			tailNetUsedInfo->totalBytes = tailNetUsedInfo->uploadBytes + tailNetUsedInfo->downloadBytes;

			curNetInfo1 = curNetInfo1->next;
			curNetInfo2 = curNetInfo2->next;
		}
		//释放列表资源
		while(totalNet1 != NULL)
		{
			curNetInfo1 = totalNet1;
			curNetInfo2 = totalNet2;
			totalNet1 = totalNet1->next;
			totalNet2 = totalNet2->next;
			vfree(curNetInfo1);
			vfree(curNetInfo2);
		}
	}
	else
	{
		//当前后两次网卡数量不一致的时，直接忽略
		totalResource->netUsed = NULL;
		//释放列表资源
		while(totalNet1 != NULL)
		{
			curNetInfo1 = totalNet1;
			totalNet1 = totalNet1->next;
			vfree(curNetInfo1);
		}
		while(totalNet2 != NULL)
		{
			curNetInfo2 = totalNet2;
			totalNet2 = totalNet2->next;
			vfree(curNetInfo2);
		}
	}
}
