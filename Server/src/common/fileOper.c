/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-04 12:35
* Last modified: 2017-07-29 10:54
* Email        : 374648064@qq.com
* Filename     : fileOper.c
* Description  : 
******************************************************/

#include "common/fileOper.h"
#include "common/strOper.h"
#include "log/logOper.h"
#include "config.h"

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
			WriteLog(0, "调用者信息\n", file, function, line);
			Error("配置文件的一行数据大小超过预设大小!\n");
			return -1;
		}
		if(_ch == '\n')
			return -1;
		data[n++] = _ch;
	}

	//read file last line
	if(n == 0)
		return 0;
	else
		return -1;
}

int KCloseFile(struct file *fd)
{
	return filp_close(fd, NULL);
}
