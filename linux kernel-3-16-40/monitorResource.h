/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-13 07:53
* Last modified: 2017-04-13 07:53
* Email        : 374648064@qq.com
* Filename     : monitorResource.h
* Description  : 
******************************************************/

#ifndef __MONITORRESOURCE_H__
#define __MONITORRESOURCE_H__

#include <linux/string.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/vmalloc.h>

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数
#define PROCESS_NAME_MAX_CHAR   100   
#define HEX_MAX_NUM            15
#define FILE_PATH_MAX_LENGTH   100    //一个文件的路径最大长度
#define BUF_SIZE 1024
#define MAX_FILENAME_LENGTH 256

struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

struct KCode_dirent {
    char name[MAX_FILENAME_LENGTH];
    char type;
    struct KCode_dirent *next;
};

struct conflictProcess
{
	int pid;
	char ProcessName[PROCESS_NAME_MAX_CHAR];
};

/**********************************
 * function: 内核态打开文件操作
 * return: struct file=成功  NULL=失败
 * @para fileName: 打开的文件名
 * @para  mode: 打开文件的模式
 *		  mode: O_RDONLY 只读方式打开
 *		  mode: O_WRONLY 只写方式打开
 *		  mode: O_RDWR 读写方式打开
 *		  mode: O_APPEND 追加方式打开
**********************************/
struct file *KOpenFile(const char* fileName, int mode);

/*********************************
 *function: 向文件写入数据
 *return: 实际写入字符个数
 *@para fd: 文件指针
 *@para data: 写入的数据
**********************************/
int KWriteFile(struct file *fd, char *data);


/*********************************
 *function: 读取文件数据
 *return: 实际读取的字符个数
 *@para fd: 文件指针
 *@para data: 存储读取的数据
 *@para size: data字符串的长度
*********************************/
int KReadFile(struct file *fd, char *data, size_t size);


/**********************************
 *function: 从文件中读取一行内容
 *return: -1 = 读取成功　　　0 = 文件末尾
 *@para fd: 文件指针
 *@para data: 存储读取的数据
***********************************/
int KReadLine(struct file *fd, char *data);


/*********************************
 * function: 关闭打开的文件
 * return: 0 = 成功
 * @oara fd: 要关闭文件的指针
*********************************/
int KCloseFile(struct file *fd);

char* decTohex(char *ch, int num);

int getPort(char *str, char *hexPort);

int judge(char *path, char *hex);

int getConflictInode(int port);

bool judgeConflictID(char *buf, int inode);

struct conflictProcess getConflictProcess(int port);

extern int vfs_opendir(const char *path);

extern struct KCode_dirent* vfs_readdir(const int fd);

extern void vfs_free_readdir(struct KCode_dirent *dir);

extern void vfs_closedir(const int fd);

extern int vfs_readlink(const char *path, char *buf, int bufsize);

#endif
