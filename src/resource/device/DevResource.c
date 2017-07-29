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
	memset(processIOData, 0, sizeof(processIOData));
	struct task_struct *p = pid_task(find_vpid(pid), PIDTYPE_PID);
	if(p == NULL)
	{
		return false;
	}
	else
	{
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
	}
}

bool getDiskStateDebug(char *diskPath, DiskStat *diskStatInfo, const char *file, const char *function, const int line)
{
	memset(diskStatInfo, 0, sizeof(DiskStat));
	struct file *fp = KOpenFile(diskPath, O_RDONLY);
	if(fp == NULL)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", diskPath, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return false;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	if(KReadLine(fp, lineData) != -1)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", diskPath, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
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
	int fdDir = vfs_opendir("/sys/block");
	if(fdDir == -1)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "打开文件夹: /sys/block  失败！\n");
		RecordLog(error_info);
		return 0;
	}
	
	struct KCode_dirent *begin;
	struct KCode_dirent *cur;
	begin = cur = vfs_readdir(fdDir);
	vfs_closedir(fdDir);
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
			WriteLog("logInfo.log", "调用者信息\n", file, function, line);
			sprintf(error_info, "%s%s%s%s%s", "从文件: ", path, "中获取磁盘信息失败！ 错误信息： ", "    ", "\n");
			RecordLog(error_info);
		}

		cur = cur->next;
	}
	//释放读取文件夹资源
	vfs_free_readdir(begin);	
	return ret_num;
}
