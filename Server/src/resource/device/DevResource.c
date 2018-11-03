/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-27 15:17
* Last modified: 2017-04-27 15:17
* Email        : 374648064@qq.com
* Filename     : DevResource.c
* Description  : 
******************************************************/

#include "resource/device/DevResource.h"

static char error_info[200];
static char subStr11[11][MAX_SUBSTR];
static char lineData[LINE_CHAR_MAX_NUM];

bool getProcessIODataDebug(pid_t pid, Process_IO_Data *processIOData, const char *file, const char *function, const int line)
{
	memset(processIOData, 0, sizeof(Process_IO_Data));
	struct task_struct *p = pid_task(find_vpid(pid), PIDTYPE_PID);
	if(p == NULL)
	{
		return false;
	}
	else
	{
		struct task_io_accounting acct = p->ioac;
		unsigned long flags;
		int result = mutex_lock_killable(&p->signal->cred_guard_mutex);
		if(result)
		{
			return false;
		}
		if(lock_task_sighand(p, &flags))
		{
			struct task_struct *t = p;
			task_io_accounting_add(&acct, &p->signal->ioac);
			while_each_thread(p, t)
				task_io_accounting_add(&acct, &t->ioac);
			unlock_task_sighand(p, &flags);
		}

		processIOData->rchar = (unsigned long long)acct.rchar;
		processIOData->wchar = (unsigned long long)acct.wchar;
		processIOData->syscr = (unsigned long long)acct.syscr;
		processIOData->syscw = (unsigned long long)acct.syscw;
		processIOData->read_bytes = (unsigned long long)acct.read_bytes;
		processIOData->write_bytes = (unsigned long long)acct.write_bytes;
		processIOData->cancelled_write_bytes = (unsigned long long)acct.cancelled_write_bytes;

		mutex_unlock(&p->signal->cred_guard_mutex);

		return true;
		/*
		task_lock(p);
		struct task_io_accounting acct = p->ioac;
		unsigned long flags;
		int result = mutex_lock_killable(&p->signal->cred_guard_mutex);
		if(result)
		{
			task_unlock(p);
			return false;
		}
		struct task_struct *t = p;
		task_io_accounting_add(&acct, &p->signal->ioac);
		while_each_thread(p, t)
			task_io_accounting_add(&acct, &t->ioac);

		processIOData->rchar = (unsigned long long)acct.rchar;
		processIOData->wchar = (unsigned long long)acct.wchar;
		processIOData->syscr = (unsigned long long)acct.syscr;
		processIOData->syscw = (unsigned long long)acct.syscw;
		processIOData->read_bytes = (unsigned long long)acct.read_bytes;
		processIOData->write_bytes = (unsigned long long)acct.write_bytes;
		processIOData->cancelled_write_bytes = (unsigned long long)acct.cancelled_write_bytes;

		mutex_unlock(&p->signal->cred_guard_mutex);
		task_unlock(p);

		return true;
		*/
	}
}

bool getDiskStateDebug(char *diskPath, DiskStat *diskStatInfo, const char *file, const char *function, const int line)
{
	memset(diskStatInfo, 0, sizeof(DiskStat));
	struct file *fp = KOpenFile(diskPath, O_RDONLY);
	if(fp == NULL)
	{
		WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", diskPath, " 失败！ 错误信息： ", "    ", "\n");
		Error(error_info);
		return false;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	if(KReadLine(fp, lineData) != -1)
	{
		WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", diskPath, " 失败！ 错误信息： ", "    ", "\n");
		Error(error_info);
		KCloseFile(fp);
		return false;
	}
	else
	{
		cutStrByLabel(lineData, ' ', subStr11, 11);
		diskStatInfo->rd_ios = ExtractNumFromStr(subStr11[0]);
		diskStatInfo->rd_merges = ExtractNumFromStr(subStr11[1]);
		diskStatInfo->rd_sectors = ExtractNumFromStr(subStr11[2]);
		diskStatInfo->rd_ticks = ExtractNumFromStr(subStr11[3]);
		diskStatInfo->wr_ios = ExtractNumFromStr(subStr11[4]);
		diskStatInfo->wr_merges = ExtractNumFromStr(subStr11[5]);
		diskStatInfo->wr_sectors = ExtractNumFromStr(subStr11[6]);
		diskStatInfo->wr_ticks = ExtractNumFromStr(subStr11[7]);
		diskStatInfo->handle_rdwr_num = ExtractNumFromStr(subStr11[8]);
		diskStatInfo->ticks = ExtractNumFromStr(subStr11[9]);
		diskStatInfo->aveq = ExtractNumFromStr(subStr11[10]);
	}

	KCloseFile(fp);
	return true;
}

int getAllDiskStateDebug(DiskInfo **beginDiskInfo, const char *file, const char *function, const int line)
{
	struct KCode_dirent *begin;
	struct KCode_dirent *cur;
	begin = cur = vfs_readdir("/sys/block");
	if(begin == NULL)
	{
		WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
		sprintf(error_info, "打开文件夹: /sys/block  失败！\n");
		Error(error_info);
		return 0;
	}
	char direntName[MAX_DIRNAME_LENGTH];
	char path[FILE_PATH_MAX_LENGTH];
	int ret_num = 0;   //返回值(磁盘的个数)
	(*beginDiskInfo) = NULL;
	DiskInfo *tailDiskInfo = NULL;
	while(cur != NULL)
	{
		//跳过.  ..  和 loop目录
		if(cur->name[0] == '.' || cur->name[0] == 'l')
		{
			cur = cur->next;
			continue;
		}
		memset(direntName, 0, MAX_DIRNAME_LENGTH);
		memset(path, 0, FILE_PATH_MAX_LENGTH);
		sprintf(path, "%s/%s/%s", "/sys/block", cur->name, "stat");
		DiskStat temp_diskStat;
		if(getDiskState(path, &temp_diskStat))
		{
			//判断是否为真实读取的磁盘，而不是虚拟磁盘
			if(temp_diskStat.ticks > 0 && temp_diskStat.aveq > 0)
			{
				if((*beginDiskInfo) == NULL)
				{
					(*beginDiskInfo) = tailDiskInfo = vmalloc(sizeof(DiskInfo));
				}
				else
				{
					tailDiskInfo = tailDiskInfo->next = vmalloc(sizeof(DiskInfo));
				}
				memset(tailDiskInfo, 0, sizeof(DiskInfo));
				tailDiskInfo->diskInfo = temp_diskStat;
				strcpy(tailDiskInfo->diskName, cur->name);
				tailDiskInfo->next = NULL;
				++ret_num;
			}
		}
		else
		{
			WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
			sprintf(error_info, "%s%s%s%s%s", "从文件: ", path, "中获取磁盘信息失败！ 错误信息： ", "    ", "\n");
			Error(error_info);
		}

		cur = cur->next;
	}
	//释放读取文件夹资源
	vfs_free_readdir(begin);	
	return ret_num;
}

bool calcDiskUsedInfo(DiskInfo *prevDiskInfo, int prevDiskNum, DiskInfo *nextDiskInfo, int nextDiskNum)
{
	DiskInfo *tempPrevDiskInfo = prevDiskInfo;
	DiskInfo *tempNextDiskInfo = nextDiskInfo;
	if(prevDiskNum == nextDiskNum && prevDiskNum != 0)
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
		while(tempPrevDiskInfo != NULL)
		{
			handle_IO_time = (tempNextDiskInfo->diskInfo.ticks - tempPrevDiskInfo->diskInfo.ticks);
			//计算每个磁盘的使用率
			if(tailIOUsedInfo == NULL)
			{
				sysResArray[currentRecordSysResIndex].ioUsed = tailIOUsedInfo = vmalloc(sizeof(IOUsedInfo));
			}
			else
			{
				tailIOUsedInfo = tailIOUsedInfo->next = vmalloc(sizeof(IOUsedInfo));
			}
			strcpy(tailIOUsedInfo->diskName, tempPrevDiskInfo->diskName);
			tailIOUsedInfo->ioUsed = handle_IO_time*100/CALC_CPU_TIME;
			tailIOUsedInfo->next = NULL;

			tempPrevDiskInfo = tempPrevDiskInfo->next;
			tempNextDiskInfo = tempNextDiskInfo->next;
		}
		//释放列表资源
		while(prevDiskInfo != NULL)
		{
			tempPrevDiskInfo = prevDiskInfo;
			tempNextDiskInfo = nextDiskInfo;
			prevDiskInfo = prevDiskInfo->next;
			nextDiskInfo = nextDiskInfo->next;
			vfree(tempPrevDiskInfo);
			vfree(tempNextDiskInfo);
		}

		return true;
	}
	else
	{
		//针对前后两次磁盘的个数不一致的情况，直接忽略这次检测
		//释放列表资源
		while(tempPrevDiskInfo != NULL)
		{
			prevDiskInfo = prevDiskInfo->next;
			vfree(tempPrevDiskInfo);
			tempPrevDiskInfo = prevDiskInfo;
		}
		while(tempNextDiskInfo != NULL)
		{
			nextDiskInfo = nextDiskInfo->next;
			vfree(tempNextDiskInfo);
			tempNextDiskInfo = nextDiskInfo;
		}

		return false;
	}
}

void getSysDiskUsedInfo()
{
	//clear old data
	currentDiskNum = 0;
	currentDiskUsedInfo = beginDiskUsedInfo;
	while(currentDiskUsedInfo != NULL)
	{
		beginDiskUsedInfo = beginDiskUsedInfo->next;
		vfree(currentDiskUsedInfo);
		currentDiskUsedInfo = beginDiskUsedInfo;
	}
	tailDiskUsedInfo = beginDiskUsedInfo = NULL;

	IOUsedInfo *diskUsed = NULL;
	IOUsedInfo *curDiskUsed = NULL;
	int i, count;
	//current record disk info(except first recorder)
	int pre_recordSysResIndex = currentRecordSysResIndex == 0? MAX_RECORD_LENGTH-1:currentRecordSysResIndex-1;
	curDiskUsed = sysResArray[pre_recordSysResIndex].ioUsed;
	int sumIOUsed = 0;
	while(curDiskUsed != NULL)
	{
		count = 0;
		sumIOUsed = 0;
		for(i = 0; i < MAX_RECORD_LENGTH; i++)
		{
			diskUsed = sysResArray[i].ioUsed;
			while(diskUsed != NULL)
			{
				if(strcasecmp(diskUsed->diskName, curDiskUsed->diskName) == 0)
				{
					sumIOUsed += diskUsed->ioUsed;
					count++;
					break;
				}
				diskUsed = diskUsed->next;
			}
		}
		if(beginDiskUsedInfo == NULL)
			beginDiskUsedInfo = tailDiskUsedInfo = vmalloc(sizeof(IOUsedInfo));
		else
			tailDiskUsedInfo = tailDiskUsedInfo->next = vmalloc(sizeof(IOUsedInfo));
		strcpy(tailDiskUsedInfo->diskName, curDiskUsed->diskName);
		tailDiskUsedInfo->ioUsed = sumIOUsed / count;
		tailDiskUsedInfo->next = NULL;
		currentDiskNum++;

		curDiskUsed = curDiskUsed->next;
	}
}
