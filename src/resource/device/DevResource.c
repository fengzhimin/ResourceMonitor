/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-27 15:17
* Last modified: 2017-04-27 15:17
* Email        : 374648064@qq.com
* Filename     : DevResource.c
* Description  : 
******************************************************/

#include "resource/device/DevResource.h"

static char io_data[512];
static char error_info[200];
static char subStr8[8][MAX_SUBSTR];
static char subStr11[11][MAX_SUBSTR];
static char lineData[LINE_CHAR_MAX_NUM];

bool getProcessIOData(char *io, Process_IO_Data *processIOData)
{
	struct file *fp = KOpenFile(io, O_RDONLY);
	if(fp == NULL)
	{
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", io, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return false;
	}

	memset(io_data, 0, 512);
	int size = KReadFile(fp, io_data, 512);
	if(size > 0)
	{
		cutStrByLabel(io_data, ':', subStr8, 8);
		processIOData->rchar = ExtractNumFromStr(subStr8[1]);
		processIOData->wchar = ExtractNumFromStr(subStr8[2]);
		processIOData->syscr = ExtractNumFromStr(subStr8[3]);
		processIOData->syscw = ExtractNumFromStr(subStr8[4]);
		processIOData->read_bytes = ExtractNumFromStr(subStr8[5]);
		processIOData->write_bytes = ExtractNumFromStr(subStr8[6]);
		processIOData->cancelled_write_bytes = ExtractNumFromStr(subStr8[7]);
		KCloseFile(fp);
		return true;
	}
	else
	{
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", io, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		KCloseFile(fp);
		return false;
	}
}

bool getDiskState(char *diskPath, DiskStat *diskStatInfo)
{
	struct file *fp = KOpenFile(diskPath, O_RDONLY);
	if(fp == NULL)
	{
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", diskPath, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return false;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	if(KReadLine(fp, lineData) != -1)
	{
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

int getAllDiskState(DiskInfo **beginDiskInfo)
{
	int fdDir = vfs_opendir("/sys/block");
	if(fdDir == -1)
	{
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
		if((*beginDiskInfo) == NULL)
		{
			(*beginDiskInfo) = tailDiskInfo = vmalloc(sizeof(DiskInfo));
			memset(tailDiskInfo, 0, sizeof(DiskInfo));
			getDiskState(path, &(tailDiskInfo->diskInfo));
			strcpy(tailDiskInfo->diskName, cur->name);
			tailDiskInfo->next = NULL;
		}
		else
		{
			tailDiskInfo = tailDiskInfo->next = vmalloc(sizeof(DiskInfo));
			memset(tailDiskInfo, 0, sizeof(DiskInfo));
			getDiskState(path, &(tailDiskInfo->diskInfo));
			strcpy(tailDiskInfo->diskName, cur->name);
			tailDiskInfo->next = NULL;
		}
		++ret_num;

		cur = cur->next;
	}
	//释放读取文件夹资源
	vfs_free_readdir(begin);	
	return true;
}
