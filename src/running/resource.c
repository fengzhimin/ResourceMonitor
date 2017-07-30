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
static ProcSchedInfo programSchedInfo;

void getSysResourceInfo()
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
		//free invalid object
		while(sysResArray[currentRecordSysResIndex].ioUsed != NULL)
		{
			tailIOUsedInfo = sysResArray[currentRecordSysResIndex].ioUsed;
			sysResArray[currentRecordSysResIndex].ioUsed = sysResArray[currentRecordSysResIndex].ioUsed->next;
			vfree(tailIOUsedInfo);
		}
		sysResArray[currentRecordSysResIndex].ioUsed = tailIOUsedInfo = NULL;
		while(curDiskInfo1 != NULL)
		{
			handle_IO_time = (curDiskInfo2->diskInfo.ticks - curDiskInfo1->diskInfo.ticks);
			//计算每个磁盘的使用率
			if(tailIOUsedInfo == NULL)
			{
				sysResArray[currentRecordSysResIndex].ioUsed = tailIOUsedInfo = vmalloc(sizeof(IOUsedInfo));
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
	sysResArray[currentRecordSysResIndex].cpuUsed = 100*(totalcpu-totalidle)/totalcpu;
	MemInfo totalMem;
	if(getTotalPM(&totalMem))
	{
		//计算内存使用率
		sysResArray[currentRecordSysResIndex].memUsed = 100*(totalMem.memTotal-totalMem.memAvailable)/totalMem.memTotal;
	}
	//获取系统的网络实时情况
	NetInfo *totalNet2;
	int totalNetInfoNum2 = getTotalNet(&totalNet2);
	NetInfo *curNetInfo1 = totalNet1;
	NetInfo *curNetInfo2 = totalNet2;
	if(totalNetInfoNum1 == totalNetInfoNum2 && totalNetInfoNum1 != 0)
	{
		NetUsedInfo *tailNetUsedInfo;
		//free invalid object
		while(sysResArray[currentRecordSysResIndex].netUsed != NULL)
		{
			tailNetUsedInfo = sysResArray[currentRecordSysResIndex].netUsed;
			sysResArray[currentRecordSysResIndex].netUsed = sysResArray[currentRecordSysResIndex].netUsed->next;
			vfree(tailNetUsedInfo);
		}
		sysResArray[currentRecordSysResIndex].netUsed = tailNetUsedInfo = NULL;
		while(curNetInfo1 != NULL)
		{
			//计算每个网卡的使用率
			if(tailNetUsedInfo == NULL)
			{
				sysResArray[currentRecordSysResIndex].netUsed = tailNetUsedInfo = vmalloc(sizeof(NetUsedInfo));
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
		beginProcRes[i].VmRss = getProgramVmRSS(currentMonitorProgPid->pid);

		/*
		 * set start record value
		 */
		programCPUTime = getProgramCPU(currentMonitorProgPid->name, currentMonitorProgPid->pid);
		programSched = getProgramSched(currentMonitorProgPid->name, currentMonitorProgPid->pid);
		programIOData = getProgramIOData(currentMonitorProgPid->name, currentMonitorProgPid->pid);
		for(j = 0; j < MAX_CHILD_PROCESS_NUM; j++)
		{
			beginProgAllRes[i].cpuTime[j] = programCPUTime.cpuTime[j];
			beginProgAllRes[i].schedInfo[j] = programSched.schedInfo[j];
			beginProgAllRes[i].ioDataBytes[j] = programIOData.ioDataBytes[j];
			beginProgAllRes[i].flags[j] = programCPUTime.flags[j] && programSched.flags[j] && programIOData.flags[j];
		}

		currentMonitorProgPid = currentMonitorProgPid->next;
	}
	Total_Cpu_Occupy_t total_cpu_occupy1;
	getTotalCPUTime(&total_cpu_occupy1);
	int total_cpu1 = total_cpu_occupy1.user + total_cpu_occupy1.nice + total_cpu_occupy1.system + total_cpu_occupy1.idle;

	//sleep CALC_CPU_TIME ms
	msleep(CALC_CPU_TIME);

	//stop hook data package
	stopHook();

	PortPackageData = PortPackageData->next;  //skip first object
	while(PortPackageData != NULL)
	{
		currentMonitorProgPid = beginMonitorProgPid;
		for(i = 0; i < MonitorAPPNameNum; i++)
		{
			for(j = 0; j < MAX_CHILD_PROCESS_NUM; j++)
			{
				if(currentMonitorProgPid->pid[j] == 0)
					break;
				memset(procPath, 0, MAX_PROCPATH);
				sprintf(procPath, "/proc/%d", currentMonitorProgPid->pid[j]);
				if(mapProcessPort(procPath, *PortPackageData))
				{
					//find process by port
					beginProcRes[i].netTotalBytes += (PortPackageData->outDataBytes + PortPackageData->inDataBytes);
					//assume a port just map a process
					goto next;
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
		programCPUTime = getProgramCPU(currentMonitorProgPid->name, currentMonitorProgPid->pid);
		programIOData = getProgramIOData(currentMonitorProgPid->name, currentMonitorProgPid->pid);
		programSched = getProgramSched(currentMonitorProgPid->name, currentMonitorProgPid->pid);
		/*
		 * Calculated CPUTime,IOData,Sched different value
		 */
		for(j = 0; j < MAX_CHILD_PROCESS_NUM; j++)
		{
			if(currentMonitorProgPid->pid[j] == 0)
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
				programSchedInfo = sub(programSched.schedInfo[j], beginProgAllRes[i].schedInfo[j]);
				beginProcRes[i].schedInfo = add(beginProcRes[i].schedInfo, programSchedInfo);
			}
		}

		currentMonitorProgPid = currentMonitorProgPid->next;
	}

	Total_Cpu_Occupy_t total_cpu_occupy2;
	getTotalCPUTime(&total_cpu_occupy2);
	int total_cpu2 = total_cpu_occupy2.user + total_cpu_occupy2.nice + total_cpu_occupy2.system + total_cpu_occupy2.idle;	
	MemInfo totalMem;
	if(getTotalPM(&totalMem))
	{
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
					currentMonitorAPP->cpuUsed[currentRecordResIndex] = 100*beginProcRes[i].cpuTime/(total_cpu2-total_cpu1);
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
				endMonitorAPP->cpuUsed[currentRecordResIndex] = 100*beginProcRes[i].cpuTime/(total_cpu2-total_cpu1);
				endMonitorAPP->schedInfo[currentRecordResIndex] = beginProcRes[i].schedInfo;
				endMonitorAPP->ioDataBytes[currentRecordResIndex] = beginProcRes[i].ioDataBytes;
				endMonitorAPP->netTotalBytes[currentRecordResIndex] = beginProcRes[i].netTotalBytes;
			}
		}

		currentRecordResIndex++;
		currentRecordResIndex %= MAX_RECORD_LENGTH;
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

	vfree(beginProcRes);
	vfree(beginProgAllRes);
}
