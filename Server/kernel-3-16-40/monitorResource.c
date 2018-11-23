/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-13 07:53
* Last modified: 2017-04-13 07:53
* Email        : 374648064@qq.com
* Filename     : monitorResource.h
* Description  : 
******************************************************/

#include <linux/monitorResource.h>

static char lineData[VFS_LINE_CHAR_MAX_NUM];
static char path[VFS_FILE_PATH_MAX_LENGTH];
static char pathfd[VFS_FILE_PATH_MAX_LENGTH];
static char buf[VFS_BUF_SIZE];
static char buflinkInfo[1024];

struct file *VFS_KOpenFile(const char* fileName, int mode)
{
	struct file *fd = NULL;
	mode |= O_CREAT;   //默认添加创建属性
	fd = filp_open(fileName, mode, 0644);
	if(IS_ERR(fd))
		fd = NULL;

	return fd;
}

int VFS_KWriteFile(struct file *fd, char *data)
{
	int _ret_value;
	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);
	_ret_value = vfs_write(fd, data, strlen(data), &fd->f_pos);
	set_fs(fs);

	return _ret_value;
}

int VFS_KReadFile(struct file *fd, char *data, size_t size)
{
	int _ret_value;
	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);
	_ret_value = vfs_read(fd, data, size, &fd->f_pos);
	set_fs(fs);

	return _ret_value;
}

int VFS_KReadLine(struct file *fd, char *data)
{
	char _ch;
	int n = 0;
	while(VFS_KReadFile(fd, &_ch, 1) == 1)
	{
		if(n >= VFS_LINE_CHAR_MAX_NUM)
		{
			printk("\033[31m[函数:%s] [文件:%s] [行数:%d] 配置文件的一行数据大小超过预设大小!\033[0m\n", __FUNCTION__, __FILE__, __LINE__);
			return -1;
		}
		if(_ch == '\n')
			return -1;
		data[n++] = _ch;
	}

	return 0;
}

int VFS_KCloseFile(struct file *fd)
{
	return filp_close(fd, NULL);
}

char* decTohex(char *ch, int num)
{
	if(num < 16)
	{
		ch[0] = num >= 10? 'A'+num-10:'0'+num;
		return ++ch;
	}
	else
	{
		ch = decTohex(ch, num/16);
		num %= 16;
		ch[0] = num >= 10? 'A'+num-10:'0'+num;
		return ++ch;
	}
}

/*
 * 格式为：0: 00000000:0050 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 16151 1 0000000000000000 100 0 0 10 0
 * 0050: 端口对应的十六进制
 * 16151: 端口对应的inode节点号
 */
int getPort(char *str, char *hexPort)
{
	int strLength = strlen(str);
	int i, point = 0, index = 0;
	int ret = 0;
	int m_point = 0;
	bool continue_space = false;
	for(i = 0; i < strLength; i++)
	{
		if(point == 2)
		{
			//跳过以0开头的十六进制数
			for( ; i < strLength; i++)
			{
				if(str[i] != '0')
					break;
			}

			//拷贝十六进制数
			while(str[i] != ' ')
			{
				hexPort[index++] = str[i++];
			}

			//查找inode号
			for( i++; i < strLength; i++)
			{
				if(m_point == 7)
				{
					if(str[i] == ' ')
						return ret;
					ret = ret*10 + str[i] - '0';   //inode
				}
				else if((str[i] == ' ' || str[i] == '\t') && !continue_space)
				{
					continue_space = true;
					m_point++;
				}
				else if(str[i] != ' ' && str[i] != '\t')
					continue_space = false;

			}
		}
		else if(str[i] == ':')
		{
			point++;
			continue;
		}
	}
	
	return -1;
}

int judge(char *path, char *hex)
{
	struct file *fp = VFS_KOpenFile(path, O_RDONLY);
	char hexPort[VFS_HEX_MAX_NUM];
	int ret = -1;
	memset(lineData, 0, VFS_LINE_CHAR_MAX_NUM);
	VFS_KReadLine(fp, lineData);
	memset(lineData, 0, VFS_LINE_CHAR_MAX_NUM);
	while(VFS_KReadLine(fp, lineData) == -1)
	{
		memset(hexPort, 0, VFS_HEX_MAX_NUM);
		ret = getPort(lineData, hexPort);
		if(strcasecmp(hexPort, hex) == 0)
		{
			VFS_KCloseFile(fp);
			return ret;   //return inode
		}
		memset(lineData, 0, VFS_LINE_CHAR_MAX_NUM);
	}
	
	VFS_KCloseFile(fp);
	return -1;  
}

int getConflictInode(int port)
{
	char hexPort[VFS_HEX_MAX_NUM] = { 0 };
	int ret;
	decTohex(hexPort, port);
	ret = judge("/proc/net/tcp", hexPort);
	if(ret != -1)
		return ret;
	else if((ret = judge("/proc/net/tcp6", hexPort)) != -1)
		return ret;
	else if((ret = judge("/proc/net/udp", hexPort)) != -1)
		return ret;
	else if((ret = judge("/proc/net/udp6", hexPort)) != -1)
		return ret;
	else
		return -1;
}

bool judgeConflictID(char *buf, int inode)
{
	int bufLength = strlen(buf);
	int i, extractNum = 0;
	for(i = 0; i < bufLength; i++)
	{
		if(buf[i] <= '9' && buf[i] >= '0')
			extractNum = extractNum*10 + buf[i] - '0';
	}
	if(inode == extractNum)
		return true;
	else
		return false;
}

struct conflictProcess getConflictProcess(int port)
{
	struct conflictProcess ret;
	struct task_struct *task, *p;
	struct list_head *ps;
	mm_segment_t fs;
	int fd, nread;
	struct linux_dirent *d;
	int bpos;
	char d_type;
	int linkSize = 0;
	//获取端口对应的inode节点号
	int inode = getConflictInode(port);
	task = &init_task;
	ret.pid = -1;
	if(inode == -1)
	{
		printk("\033[31mget inode failure!(port:%d)\033[0m\n", port);
		return ret;
	}
	fs = get_fs();
	set_fs(KERNEL_DS);
	//遍历所有正在运行的进程的信息
	list_for_each(ps, &task->tasks)
	{
		p = list_entry(ps, struct task_struct, tasks);
		memset(path, 0, VFS_FILE_PATH_MAX_LENGTH);
		sprintf(path, "/proc/%d/fd", p->pid);
		//打开/proc/pid/fd文件夹
		fd = sys_open(path, O_RDONLY | O_DIRECTORY, 0);
		if (fd >= 0)
		{
			for ( ; ; ) 
			{
				//读取/proc/pid/fd文件夹下的内容
				memset(buf, 0, VFS_BUF_SIZE);
				nread = sys_getdents(fd, buf, VFS_BUF_SIZE);
				if (nread < 0)
				{
					printk("\033[31mgetdents failure! error code = %d\033[0m\n", nread);
					break;
				}

				if (nread == 0)
				    break;

				for (bpos = 0; bpos < nread; bpos += d->d_reclen) 
				{
				    d = (struct linux_dirent *) (buf + bpos);
				    d_type = *(buf + bpos + d->d_reclen - 1);
				    if(d_type == DT_LNK)
					{
						memset(pathfd, 0, VFS_FILE_PATH_MAX_LENGTH);
						sprintf(pathfd, "%s/%s", path, d->d_name);
						linkSize = sys_readlink(pathfd, buflinkInfo, 1024);
						if(linkSize < 0 || linkSize > 1024)
							continue;
						buflinkInfo[linkSize] = '\0';
						//比较对应进程的节点号是否为已经占用端口对应的节点号
						if(judgeConflictID(buflinkInfo, inode))
						{
							ret.pid = p->pid;
							memset(ret.ProcessName, 0, VFS_PROCESS_NAME_MAX_CHAR);
							strcpy(ret.ProcessName, p->comm);
							sys_close(fd);

							return ret;
						}
				    }
				}
			}
			sys_close(fd);
		}
		
	}
	set_fs(fs);

	return ret;
}

struct KCode_dirent* vfs_readdir(const char *path)
{
	int fd = 0;
	int nread;
	struct linux_dirent *d;
	int bpos;
	char d_type;
	int dirent_size;
	struct KCode_dirent *begin = NULL;
	struct KCode_dirent * cur = NULL;
	int num = 0;
	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);
	fd = sys_open(path, O_RDONLY | O_DIRECTORY, 0);
	if(fd < 0)
		return NULL;
	dirent_size = sizeof(struct KCode_dirent);
	cur = begin = vmalloc(dirent_size);
	memset(begin, 0 ,dirent_size);
	begin->next = NULL;
	//printk("begin = %d\t begin->name = %s\n", begin, begin->name);
	while(1)
	{
		memset(buf, 0, VFS_BUF_SIZE);
		nread = sys_getdents(fd, buf, VFS_BUF_SIZE);
		if (nread < 0)
		{
			//printk("getdents failure! nread = %d\n", nread);
			break;
		}

		if (nread == 0)
		    break;

		for (bpos = 0; bpos < nread;) 
		{
		    d = (struct linux_dirent *) (buf + bpos);
		    //printk("%8ld  ", d->d_ino);
		    d_type = *(buf + bpos + d->d_reclen - 1);
		    bpos += d->d_reclen;
		    if(num != 0)
		    {
				cur = cur->next = vmalloc(dirent_size);
				memset(cur, 0, dirent_size);
				cur->next = NULL;
		    }
		    strcpy(cur->name, d->d_name);
		    //printk("cur = %d\t cur->name = %s\n", cur, cur->name);
		    cur->type = d_type;
		    num++;
		}
	}
	sys_close(fd);
	set_fs(fs);
	return begin;
}
EXPORT_SYMBOL(vfs_readdir);

extern void vfs_free_readdir(struct KCode_dirent *dir)
{
	struct KCode_dirent *temp = NULL;
	while(dir->next != NULL)
	{
		temp = dir;
		dir = dir->next;
		vfree(temp);
	}
	vfree(dir);
}
EXPORT_SYMBOL(vfs_free_readdir);

/*
void vfs_readdir(const char *path)
{
	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);
	int fd, nread;
	char buf[BUF_SIZE];
	struct linux_dirent *d;
	int bpos;
	char d_type;
	fd = sys_open(path, O_RDONLY | O_DIRECTORY, 0);
	if (fd < 0)
	{
		printk("open file %s failure!\n", path);
	}
	else
	{
		for ( ; ; ) 
		{
			nread = sys_getdents(fd, buf, BUF_SIZE);
			if (nread < 0)
			{
				printk("getdents failure! nread = %d\n", nread);
				break;
			}

			if (nread == 0)
			    break;

			printk("--------------- nread=%d ---------------\n", nread);
			printk("i-node#  file type  d_reclen  d_off   d_name\n");
			for (bpos = 0; bpos < nread;) 
			{
			    d = (struct linux_dirent *) (buf + bpos);
			    printk("%8ld  ", d->d_ino);
			    d_type = *(buf + bpos + d->d_reclen - 1);
			    printk("%-10s ", (d_type == DT_REG) ?  "regular" :
					     (d_type == DT_DIR) ?  "directory" :
					     (d_type == DT_FIFO) ? "FIFO" :
					     (d_type == DT_SOCK) ? "socket" :
					     (d_type == DT_LNK) ?  "symlink" :
					     (d_type == DT_BLK) ?  "block dev" :
					     (d_type == DT_CHR) ?  "char dev" : "???");
			    printk("%4d %10lld  %s\n", d->d_reclen,
				    (long long) d->d_off, d->d_name);
			    bpos += d->d_reclen;
			}
		}
	}
	sys_close(fd);
	set_fs(fs);
}
EXPORT_SYMBOL(vfs_readdir);
*/

int vfs_readlink(const char *path, char *buf, int bufsize)
{
	int ret = 0;
	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);
/*
	struct kstat stat;
	vfs_lstat(path, &stat);
	printk("stat.ino = %d\n", stat.ino);
	printk("stat.dev = %d\n", stat.dev);
	printk("stat.mode = %d\n", stat.mode);
*/
	ret = sys_readlink(path, buf, bufsize);
	set_fs(fs);
	return ret;
}
EXPORT_SYMBOL(vfs_readlink);

long vfs_socket(int family, int type, int protocol)
{
	int ret = 0;
	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);
	ret = sys_socket(family, type, protocol);
	set_fs(fs);
	return ret;
}
EXPORT_SYMBOL(vfs_socket);

long vfs_socketClose(unsigned int sockfd)
{
	int ret = 0;
	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);
	ret = sys_close(sockfd);
	set_fs(fs);
	return ret;
}
EXPORT_SYMBOL(vfs_socketClose);

long vfs_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	mm_segment_t fs;
	fs = get_fs();
	set_fs(KERNEL_DS);
    ret = sys_ioctl(fd, cmd, arg);
	set_fs(fs);
	return ret;
}
EXPORT_SYMBOL(vfs_ioctl);
