/******************************************************
* Author       : fengzhimin
* Create       : 2017-03-18 15:06
* Last modified: 2017-03-18 15:06
* Email        : 374648064@qq.com
* Filename     : dirOper.c
* Description  : 
******************************************************/

#include "common/dirOper.h"
#include "log/logOper.h"

static char error_info[200];

int Is_Dir(const char *path)
{
	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);
	struct kstat st;
	if(vfs_stat(path, &st) == -1)
	{
		sprintf(error_info, "%s%s%s%s%s", "判断：", path, "是否为文件夹时错误，", "    ", "\n");
		RecordLog(error_info);
		return 0;
	}
	set_fs(fs);

	if(S_ISDIR(st.mode))
		return -1;
	else if(S_ISREG(st.mode))
		return -2;
	else
		return -3;
}
