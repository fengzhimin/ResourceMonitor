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

int getProcessIOData(char *io, Process_IO_Data *processIOData)
{
	struct file *fp = KOpenFile(io, O_RDONLY);
	if(fp == NULL)
	{
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", io, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return -1;
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
		return 1;
	}
	else
	{
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", io, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		KCloseFile(fp);
		return -1;
	}
}
