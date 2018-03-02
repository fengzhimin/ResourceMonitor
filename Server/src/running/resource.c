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
	Total_Cpu_Occupy_t total_cpu_occupy1;
	getTotalCPUTime(&total_cpu_occupy1);
	unsigned long long total_cpu1 = total_cpu_occupy1.user + total_cpu_occupy1.nice + total_cpu_occupy1.system + total_cpu_occupy1.idle;
	//获取系统的网络使用情况
	NetInfo *totalNet1;
	int totalNetInfoNum1 = getAllNetState(&totalNet1);
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
	unsigned long long total_cpu2 = total_cpu_occupy2.user + total_cpu_occupy2.nice + total_cpu_occupy2.system + total_cpu_occupy2.idle;	
	//计算总CPU使用率
	unsigned long long totalcpu = total_cpu2 - total_cpu1;
	unsigned long long totalidle = total_cpu_occupy2.idle - total_cpu_occupy1.idle;
	sysResArray[currentRecordSysResIndex].cpuUsed = 100*(totalcpu-totalidle)/totalcpu;
	MemInfo totalMem;
	if(getTotalPM(&totalMem))
	{
		//计算内存使用率
		sysResArray[currentRecordSysResIndex].memUsed = 100*(totalMem.memTotal-totalMem.memAvailable)/totalMem.memTotal;
		sysResArray[currentRecordSysResIndex].swapUsed = 100*(totalMem.totalswap-totalMem.freeswap)/totalMem.totalswap;
	}
	//获取系统的网络实时情况
	NetInfo *totalNet2;
	int totalNetInfoNum2 = getAllNetState(&totalNet2);
	NetInfo *curNetInfo1 = totalNet1;
	NetInfo *curNetInfo2 = totalNet2;
	unsigned long long totalPackage = 0;
	unsigned long long totalBytes = 0;
	int speed;
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
			speed = getNetCardSpeed(curNetInfo1->netCardName);
			//计算每个网卡的使用率
			if(tailNetUsedInfo == NULL)
			{
				sysResArray[currentRecordSysResIndex].netUsed = tailNetUsedInfo = vmalloc(sizeof(NetUsedInfo));
			}
			else
			{
				tailNetUsedInfo = tailNetUsedInfo->next = vmalloc(sizeof(NetUsedInfo));
			}
			strcpy(tailNetUsedInfo->netCardName, curNetInfo1->netCardName);
			tailNetUsedInfo->next = NULL;
			//计算出来的是百分比
			if(speed != 0)
			{
				totalPackage = curNetInfo2->netCardInfo.uploadPackage - curNetInfo1->netCardInfo.uploadPackage + curNetInfo2->netCardInfo.downloadPackage - curNetInfo1->netCardInfo.downloadPackage;
				totalBytes = curNetInfo2->netCardInfo.uploadBytes - curNetInfo1->netCardInfo.uploadBytes + curNetInfo2->netCardInfo.downloadBytes - curNetInfo1->netCardInfo.downloadBytes;
				tailNetUsedInfo->netUsed = totalBytes*8/(speed*10000);
			}
			else
				tailNetUsedInfo->netUsed = 0;

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
	Total_Cpu_Occupy_t total_cpu_occupy1;
	getTotalCPUTime(&total_cpu_occupy1);
	int total_cpu1 = total_cpu_occupy1.user + total_cpu_occupy1.nice + total_cpu_occupy1.system + total_cpu_occupy1.idle;

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
					currentMonitorAPP->swapUsed[currentRecordResIndex] = 100*beginProcRes[i].swap/totalMem.totalswap;
					currentMonitorAPP->maj_flt[currentRecordResIndex] = beginProcRes[i].maj_flt;
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
				endMonitorAPP->swapUsed[currentRecordResIndex] = 100*beginProcRes[i].swap/totalMem.totalswap;
				endMonitorAPP->maj_flt[currentRecordResIndex] = beginProcRes[i].maj_flt;
				endMonitorAPP->cpuUsed[currentRecordResIndex] = 100*beginProcRes[i].cpuTime/(total_cpu2-total_cpu1);
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

		/*
		 * set process resource used when the process running is normal
		 */
		//先查找是否有进程当前存在延时
		currentMonitorAPP = beginMonitorAPP;
		bool isConflict = false;
		while(currentMonitorAPP != NULL)
		{
			int aveWait_sum = 0;
			int aveIOWait_sum = 0;
			
			int count;
			for(count = 0; count < MAX_RECORD_LENGTH; count++)
			{
				aveWait_sum += currentMonitorAPP->schedInfo[count].wait_sum;
				aveIOWait_sum += currentMonitorAPP->schedInfo[count].iowait_sum;
			}

			if(aveWait_sum >= PROC_MAX_SCHED.wait_sum || aveIOWait_sum >= PROC_MAX_SCHED.iowait_sum)
			{
				//存在进程在某一时刻延时
				isConflict = true;
				break;
			}

			currentMonitorAPP = currentMonitorAPP->next;
		}
		
		//判断此时是否有软件延时
		if(! isConflict)
		{
			/*
			 * 不存在软件延时
			 * 更新正常运行时软件资源使用情况
			 */
			currentMonitorAPP = beginMonitorAPP;
			int sumCPU, sumMEM, sumSWAP;
			unsigned long long sumIOData, sumNetData;
			int count;
			while(currentMonitorAPP != NULL)
			{
				sumCPU = sumMEM = sumSWAP = 0;
				sumIOData = sumNetData = 0;
				for(count = 0; count < MAX_RECORD_LENGTH; count++)
				{
					sumCPU += currentMonitorAPP->cpuUsed[count];
					sumMEM += currentMonitorAPP->memUsed[count];
					sumSWAP += currentMonitorAPP->swapUsed[count];
					sumIOData += currentMonitorAPP->ioDataBytes[count];
					sumNetData += currentMonitorAPP->netTotalBytes[count];
				}

				currentMonitorAPP->normalResUsed.cpuUsed = sumCPU / MAX_RECORD_LENGTH;
				currentMonitorAPP->normalResUsed.memUsed = sumMEM / MAX_RECORD_LENGTH;
				currentMonitorAPP->normalResUsed.swapUsed = sumSWAP / MAX_RECORD_LENGTH;
				currentMonitorAPP->normalResUsed.ioDataBytes = sumIOData / MAX_RECORD_LENGTH;
				currentMonitorAPP->normalResUsed.netTotalBytes = sumNetData / MAX_RECORD_LENGTH;

				currentMonitorAPP = currentMonitorAPP->next;
			}
		}

		currentRecordResIndex++;
		currentRecordResIndex %= MAX_RECORD_LENGTH;
	}

	vfree(beginProcRes);
	vfree(beginProgAllRes);
}
