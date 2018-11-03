/******************************************************
* Author       : fengzhimin
* Create       : 2016-12-29 16:32
* Last modified: 2017-04-10 21:33
* Email        : 374648064@qq.com
* Filename     : resource.c
* Description  : 
******************************************************/
#include "running/resource.h"

static char procPath[MAX_PROCPATH];    //example: /proc/1234
static ProgAllRes programCPUTime;
static ProgAllRes programIOData;
static ProgAllRes programSched;
static ProgAllRes programMaj_flt;
static ProcSchedInfo programSchedInfo;
static Process_Mem_Info programMemInfo;
static int _port[MAX_PORT_NUM];

void getSysResourceInfo()
{
	//获取前一时刻系统的CPU使用情况
	Total_Cpu_Occupy_t prev_cpu_occupy;
	getTotalCPUTime(&prev_cpu_occupy);
	unsigned long long prevCPUData = prev_cpu_occupy.user + prev_cpu_occupy.nice + prev_cpu_occupy.system + prev_cpu_occupy.idle;
	//获取前一时刻系统的网络使用情况
	NetInfo *prevNetInfo;
	int prevNetInfoNum = getAllNetState(&prevNetInfo);
	//获取前一时刻系统的磁盘使用情况
	DiskInfo *prevDiskInfo;
	int prevDiskInfoNum = getAllDiskState(&prevDiskInfo);

	//隔一段时间
	msleep(CALC_CPU_TIME);

	//获取下一时刻系统的CPU使用情况
	Total_Cpu_Occupy_t next_cpu_occupy;
	getTotalCPUTime(&next_cpu_occupy);
	unsigned long long nextCPUData = next_cpu_occupy.user + next_cpu_occupy.nice + next_cpu_occupy.system + next_cpu_occupy.idle;	
	//获取下一时刻系统的网络使用情况
	NetInfo *nextNetInfo;
	int nextNetInfoNum = getAllNetState(&nextNetInfo);
	//获取下一时刻系统的磁盘使用情况
	DiskInfo *nextDiskInfo;
	int nextDiskInfoNum = getAllDiskState(&nextDiskInfo);

	//计算总CPU使用率
	unsigned long long totalcpu = nextCPUData - prevCPUData;
	unsigned long long totalidle = next_cpu_occupy.idle - prev_cpu_occupy.idle;
	sysResArray[currentRecordSysResIndex].cpuUsed = 100*(totalcpu-totalidle)/totalcpu;
	//计算总的系统磁盘使用率
	calcDiskUsedInfo(prevDiskInfo, prevDiskInfoNum, nextDiskInfo, nextDiskInfoNum);
	//计算总的系统网卡使用率
	calcNetUsedInfo(nextNetInfo, nextNetInfoNum, prevNetInfo, prevNetInfoNum);
	//计算物理内存和交换空间的使用率
	MemInfo totalMem;
	if(getTotalPM(&totalMem))
	{
		sysResArray[currentRecordSysResIndex].memUsed = 100*(totalMem.memTotal-totalMem.memAvailable)/totalMem.memTotal;
		sysResArray[currentRecordSysResIndex].swapUsed = 100*(totalMem.totalswap-totalMem.freeswap)/totalMem.totalswap;
	}

	currentRecordSysResIndex++;
	currentRecordSysResIndex %= MAX_RECORD_LENGTH;
}

void getUserLayerAPP()
{
#if (MONITOR_TYPE == 1)
	/*
	 * get all user layer application
	 */
	clearMonitorAPPName();
	getAllMonitorAPPName();
#endif

#if (MONITOR_TYPE == 0)
	getMonitorSoftWare();
#endif

	/*
	 * get monitor program's all process
	 */
	clearMonitorProgPid();
	getAllMonitorProgPid();

	//start hook data package
	startHook();

	ProcRes *beginProcRes = vmalloc(sizeof(ProcRes)*MonitorAPPNameNum);
	memset(beginProcRes, 0, sizeof(ProcRes)*MonitorAPPNameNum);
	ProgAllRes *beginProgAllRes = vmalloc(sizeof(ProgAllRes)*MonitorAPPNameNum);
	memset(beginProgAllRes, 0, sizeof(ProgAllRes)*MonitorAPPNameNum);

	currentMonitorProgPid = beginMonitorProgPid;
	int i, j;
	for(i = 0; i < MonitorAPPNameNum; i++)
	{
		strcpy(beginProcRes[i].name, currentMonitorProgPid->name);
		beginProcRes[i].pgid = currentMonitorProgPid->pgid;
		getProgramMemInfo(currentMonitorProgPid->childPid, &programMemInfo);
		beginProcRes[i].VmRss = programMemInfo.rss;
		beginProcRes[i].swap = programMemInfo.swap;

		/*
		 * set start record value
		 */
		programCPUTime = getProgramCPU(currentMonitorProgPid->name, currentMonitorProgPid->childPid);
		programSched = getProgramSched(currentMonitorProgPid->name, currentMonitorProgPid->childPid);
		programIOData = getProgramIOData(currentMonitorProgPid->name, currentMonitorProgPid->childPid);
		programMaj_flt = getProgramMaj_flt(currentMonitorProgPid->name, currentMonitorProgPid->childPid);
		for(j = 0; j < MAX_CHILD_PROCESS_NUM; j++)
		{
			beginProgAllRes[i].cpuTime[j] = programCPUTime.cpuTime[j];
			beginProgAllRes[i].schedInfo[j] = programSched.schedInfo[j];
			beginProgAllRes[i].ioDataBytes[j] = programIOData.ioDataBytes[j];
			beginProgAllRes[i].maj_flt[j] = programMaj_flt.maj_flt[j];
			beginProgAllRes[i].flags[j] = programCPUTime.flags[j] && programSched.flags[j] && programIOData.flags[j];
		}

		currentMonitorProgPid = currentMonitorProgPid->next;
	}
	//获取前一时刻系统的CPU使用情况
	Total_Cpu_Occupy_t prev_cpu_occupy;
	getTotalCPUTime(&prev_cpu_occupy);
	unsigned long long prevCPUData = prev_cpu_occupy.user + prev_cpu_occupy.nice + prev_cpu_occupy.system + prev_cpu_occupy.idle;
	//获取前一时刻系统的网络使用情况
	NetInfo *prevNetInfo;
	int prevNetInfoNum = getAllNetState(&prevNetInfo);
	//获取前一时刻系统的磁盘使用情况
	DiskInfo *prevDiskInfo;
	int prevDiskInfoNum = getAllDiskState(&prevDiskInfo);

	//sleep CALC_CPU_TIME ms
	msleep(CALC_CPU_TIME);

	//stop hook data package
	stopHook();

	//set sockflag value
	currentMonitorProgPid = beginMonitorProgPid;
	for(i = 0; i < MonitorAPPNameNum; i++)
	{
		for(j = 0; j < MAX_CHILD_PROCESS_NUM; j++)
		{
			if(currentMonitorProgPid->childPid[j] == 0)
				break;
			memset(procPath, 0, MAX_PROCPATH);
			sprintf(procPath, "/proc/%d", currentMonitorProgPid->childPid[j]);
			if(IsSocketLink(procPath, _port))
			{
				currentMonitorProgPid->sockflag = true;
				int n;
				for(n = 0; n < MAX_PORT_NUM; n++)
				{
					if(_port[n] != 0)
					{
						if(currentMonitorProgPid->usePort_index == MAX_PORT_NUM)
							break;
						currentMonitorProgPid->usePort[currentMonitorProgPid->usePort_index++] = _port[n];
					}
				}
				break;
			}
		}

		currentMonitorProgPid = currentMonitorProgPid->next;
	}
	
	PortPackageData = PortPackageData->next;  //skip first object
	while(PortPackageData != NULL)
	{
		currentMonitorProgPid = beginMonitorProgPid;
		int portinode = getInodeByPort(PortPackageData->port, PortPackageData->protocol);
		for(i = 0; i < MonitorAPPNameNum; i++)
		{
			if(currentMonitorProgPid->sockflag)
			{
				int n;
				for(n = 0; n < currentMonitorProgPid->usePort_index; n++)
				{
					if(currentMonitorProgPid->usePort[n] == 0)
						break;
					if(currentMonitorProgPid->usePort[n] == portinode)
					{
						//find process by port
						beginProcRes[i].netTotalBytes += (PortPackageData->outDataBytes + PortPackageData->inDataBytes);
						//assume a port only map a process
						goto next;
					}	
				}
			}

			currentMonitorProgPid = currentMonitorProgPid->next;
		}
	next:
		PortPackageData = PortPackageData->next;
	}
	

	bool processValid = false;
	int processNum = 0;
	currentMonitorProgPid = beginMonitorProgPid;
	for(i = 0; i < MonitorAPPNameNum; i++)
	{
		processValid = false;
		processNum = 0;
		/*
		 * set start record value
		 */
		programCPUTime = getProgramCPU(currentMonitorProgPid->name, currentMonitorProgPid->childPid);
		programIOData = getProgramIOData(currentMonitorProgPid->name, currentMonitorProgPid->childPid);
		programSched = getProgramSched(currentMonitorProgPid->name, currentMonitorProgPid->childPid);
		programMaj_flt = getProgramMaj_flt(currentMonitorProgPid->name, currentMonitorProgPid->childPid);
		/*
		 * Calculated CPUTime,IOData,Sched different value
		 */
		for(j = 0; j < MAX_CHILD_PROCESS_NUM; j++)
		{
			if(currentMonitorProgPid->childPid[j] == 0)
			{
				beginProcRes[i].flags = processValid;
				beginProcRes[i].processNum = processNum;
				break;
			}
			if(beginProgAllRes[i].flags[j] && programCPUTime.flags[j] && programIOData.flags[j] && programSched.flags[j])
			{
				/*
				 * There is at least one process
				 */
				processValid = true;
				processNum++;
				beginProcRes[i].cpuTime += (programCPUTime.cpuTime[j] - beginProgAllRes[i].cpuTime[j]);
				beginProcRes[i].ioDataBytes += (programIOData.ioDataBytes[j] - beginProgAllRes[i].ioDataBytes[j]);
				beginProcRes[i].maj_flt += (programMaj_flt.maj_flt[j] - beginProgAllRes[i].maj_flt[j]);
				programSchedInfo = sub(programSched.schedInfo[j], beginProgAllRes[i].schedInfo[j]);
				beginProcRes[i].schedInfo = add(beginProcRes[i].schedInfo, programSchedInfo);
			}
		}

		currentMonitorProgPid = currentMonitorProgPid->next;
	}

	//获取下一时刻系统的CPU使用情况
	Total_Cpu_Occupy_t next_cpu_occupy;
	getTotalCPUTime(&next_cpu_occupy);
	unsigned long long nextCPUData = next_cpu_occupy.user + next_cpu_occupy.nice + next_cpu_occupy.system + next_cpu_occupy.idle;	
	//获取下一时刻系统的网络使用情况
	NetInfo *nextNetInfo;
	int nextNetInfoNum = getAllNetState(&nextNetInfo);
	//获取下一时刻系统的磁盘使用情况
	DiskInfo *nextDiskInfo;
	int nextDiskInfoNum = getAllDiskState(&nextDiskInfo);
	//计算总CPU使用率
	unsigned long long totalcpu = nextCPUData - prevCPUData;
	unsigned long long totalidle = next_cpu_occupy.idle - prev_cpu_occupy.idle;
	sysResArray[currentRecordSysResIndex].cpuUsed = 100*(totalcpu-totalidle)/totalcpu;
	//计算总的系统磁盘使用率
	calcDiskUsedInfo(prevDiskInfo, prevDiskInfoNum, nextDiskInfo, nextDiskInfoNum);
	//计算总的系统网卡使用率
	calcNetUsedInfo(nextNetInfo, nextNetInfoNum, prevNetInfo, prevNetInfoNum);
	//计算物理内存和交换空间的使用率

	//更新索引值
	currentRecordSysResIndex++;
	currentRecordSysResIndex %= MAX_RECORD_LENGTH;

	MemInfo totalMem;
	if(getTotalPM(&totalMem))
	{
		//计算系统总的内存和交换分区的使用率
		sysResArray[currentRecordSysResIndex].memUsed = 100*(totalMem.memTotal-totalMem.memAvailable)/totalMem.memTotal;
		sysResArray[currentRecordSysResIndex].swapUsed = 100*(totalMem.totalswap-totalMem.freeswap)/totalMem.totalswap;
		/*
		 * init beginMonitorAPP flags is equal to false;
		 */
		currentMonitorAPP = beginMonitorAPP;
		while(currentMonitorAPP != NULL)
		{
			currentMonitorAPP->flags = false;
			currentMonitorAPP = currentMonitorAPP->next;
		}
		/*
		 * update MonitorApp list
		 */
		for(i = 0; i < MonitorAPPNameNum; i++)
		{
			if(!beginProcRes[i].flags)
				continue;

			currentMonitorAPP = beginMonitorAPP;
			while(currentMonitorAPP != NULL)
			{
				/*
				 * find the program to be updated
				 */
				if(strcasecmp(currentMonitorAPP->name, beginProcRes[i].name) == 0 && currentMonitorAPP->pgid == beginProcRes[i].pgid)
				{
					currentMonitorAPP->flags = true;
					currentMonitorAPP->processNum = beginProcRes[i].processNum;
					currentMonitorAPP->memUsed[currentRecordResIndex] = 100*beginProcRes[i].VmRss/totalMem.memTotal;
					currentMonitorAPP->swapUsed[currentRecordResIndex] = 100*beginProcRes[i].swap/totalMem.totalswap;
					currentMonitorAPP->maj_flt[currentRecordResIndex] = beginProcRes[i].maj_flt;
					currentMonitorAPP->cpuUsed[currentRecordResIndex] = 100*beginProcRes[i].cpuTime/(nextCPUData-prevCPUData);
					currentMonitorAPP->schedInfo[currentRecordResIndex] = beginProcRes[i].schedInfo;
					currentMonitorAPP->ioDataBytes[currentRecordResIndex] = beginProcRes[i].ioDataBytes;
					currentMonitorAPP->netTotalBytes[currentRecordResIndex] = beginProcRes[i].netTotalBytes;

					break;
				}
				currentMonitorAPP = currentMonitorAPP->next;
			}

			/*
			 * new a Monitor program
			 */
			if(currentMonitorAPP == NULL)
			{
				if(beginMonitorAPP == NULL)
				{
					beginMonitorAPP = endMonitorAPP = currentMonitorAPP = vmalloc(sizeof(ResUtilization));
					memset(endMonitorAPP, 0, sizeof(ResUtilization));
				}
				else
				{
					currentMonitorAPP = endMonitorAPP;
					endMonitorAPP = endMonitorAPP->next = vmalloc(sizeof(ResUtilization));
					memset(endMonitorAPP, 0, sizeof(ResUtilization));
					endMonitorAPP->pre = currentMonitorAPP;
				}

				strcpy(endMonitorAPP->name, beginProcRes[i].name);
				endMonitorAPP->pgid = beginProcRes[i].pgid;
				endMonitorAPP->processNum = beginProcRes[i].processNum;
				endMonitorAPP->flags = true;
				endMonitorAPP->memUsed[currentRecordResIndex] = 100*beginProcRes[i].VmRss/totalMem.memTotal;
				endMonitorAPP->swapUsed[currentRecordResIndex] = 100*beginProcRes[i].swap/totalMem.totalswap;
				endMonitorAPP->maj_flt[currentRecordResIndex] = beginProcRes[i].maj_flt;
				endMonitorAPP->cpuUsed[currentRecordResIndex] = 100*beginProcRes[i].cpuTime/(nextCPUData-prevCPUData);
				endMonitorAPP->schedInfo[currentRecordResIndex] = beginProcRes[i].schedInfo;
				endMonitorAPP->ioDataBytes[currentRecordResIndex] = beginProcRes[i].ioDataBytes;
				endMonitorAPP->netTotalBytes[currentRecordResIndex] = beginProcRes[i].netTotalBytes;
			}
		}

		/*
		 * delete the exit program
		 */
		ResUtilization *delete_point = NULL;
		currentMonitorAPP = beginMonitorAPP;
		while(currentMonitorAPP != NULL)
		{
			if(!currentMonitorAPP->flags)
			{
				delete_point = currentMonitorAPP;
				if(currentMonitorAPP->pre == NULL)
				{
					//first object
					if(currentMonitorAPP->next != NULL)
						currentMonitorAPP->next->pre = NULL;
					currentMonitorAPP = beginMonitorAPP = beginMonitorAPP->next;
				}
				else if(currentMonitorAPP->next != NULL)
				{
					//middle object
					currentMonitorAPP->pre->next = currentMonitorAPP->next;
					currentMonitorAPP->next->pre = currentMonitorAPP->pre;
					currentMonitorAPP = currentMonitorAPP->next;
				}
				else
				{
					//last object
					endMonitorAPP = endMonitorAPP->pre;
					currentMonitorAPP = endMonitorAPP->next = NULL;
				}
				vfree(delete_point);
				continue;
			}
			currentMonitorAPP = currentMonitorAPP->next;
		}

		currentRecordResIndex++;
		currentRecordResIndex %= MAX_RECORD_LENGTH;
	}

	vfree(beginProcRes);
	vfree(beginProgAllRes);
}
