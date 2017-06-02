/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-04 12:35
* Last modified: 2017-03-17 16:52
* Email        : 374648064@qq.com
* Filename     : fileOper.c
* Description  : 
******************************************************/

#include "common/fileOper.h"
#include "common/strOper.h"
#include "log/logOper.h"
#include "config.h"

static char error_info[200];
static char lineInfo[LINE_CHAR_MAX_NUM];

struct file *KOpenFile(const char* fileName, int mode)
{
	struct file *fd = NULL;
	mode |= O_CREAT;   //默认添加创建属性
	fd = filp_open(fileName, mode, 0644);
	if(IS_ERR(fd))
		fd = NULL;

	return fd;
}

int KWriteFile(struct file *fd, char *data)
{
	int _ret_value;
	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);
	_ret_value = vfs_write(fd, data, strlen(data), &fd->f_pos);
	set_fs(fs);

	return _ret_value;
}

int KReadFile(struct file *fd, char *data, size_t size)
{
	int _ret_value;
	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);
	_ret_value = vfs_read(fd, data, size, &fd->f_pos);
	set_fs(fs);

	return _ret_value;
}

int KReadLineDebug(struct file *fd, char *data, const char *file, const char *function, const int line)
{
	char _ch;
	int n = 0;
	while(KReadFile(fd, &_ch, 1) == 1)
	{
		if(n >= LINE_CHAR_MAX_NUM)
		{
			WriteLog("logInfo.log", "调用者信息\n", file, function, line);
			RecordLog("配置文件的一行数据大小超过预设大小!\n");
			return -1;
		}
		if(_ch == '\n')
			return -1;
		data[n++] = _ch;
	}

	return 0;
}

int KCloseFile(struct file *fd)
{
	return filp_close(fd, NULL);
}

void RemoveNote(char *fileName, char *fileNameCopy)
{
	struct file *fd = KOpenFile(fileName, O_RDONLY);
	if(fd == NULL)
	{
		sprintf(error_info, "%s%s%s%s%s", "文件: ", fileName, " 打开失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return ;
	}
	struct file *fdCopy = KOpenFile(fileNameCopy, O_APPEND | O_RDWR);
	if(fd == NULL)
	{
		sprintf(error_info, "%s%s%s%s%s", "创建文件: ", fileNameCopy, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		return ;
	}
	memset(lineInfo, 0, LINE_CHAR_MAX_NUM);
	while(KReadLine(fd, lineInfo) == -1)
	{
		if(!JudgeNote(lineInfo))
			KWriteFile(fdCopy, lineInfo);	
		memset(lineInfo, 0, LINE_CHAR_MAX_NUM);
	}

	KCloseFile(fd);
	KCloseFile(fdCopy);

}

