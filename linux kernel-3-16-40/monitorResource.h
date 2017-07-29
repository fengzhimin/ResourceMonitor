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
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>

#define VFS_LINE_CHAR_MAX_NUM      1024   //一行最大字符个数
#define VFS_PROCESS_NAME_MAX_CHAR   100   
#define VFS_HEX_MAX_NUM            15
#define VFS_FILE_PATH_MAX_LENGTH   100    //一个文件的路径最大长度
#define VFS_BUF_SIZE 1024
#define VFS_MAX_FILENAME_LENGTH 256

struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

struct KCode_dirent {
    char name[VFS_MAX_FILENAME_LENGTH];
    char type;
    struct KCode_dirent *next;
};

/******************************************
 * func: 存放冲突进程的信息
 * pid: 进程ID
 * ProcessName: 进程的名称
******************************************/
struct conflictProcess
{
	int pid;
	char ProcessName[VFS_PROCESS_NAME_MAX_CHAR];
};

/******************************************
 * func:　定义端口冲突进程的信息
 * port:　冲突的端口号
 * currentProcess: 被运行的进程的信息
 * runningProcess: 已经占用端口的进程信息
******************************************/
typedef struct ConflictPortProcessInfo
{
	unsigned int port;
	struct conflictProcess currentProcess;
	struct conflictProcess runningProcess;
} ConflictPortProcInfo;

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
struct file *VFS_KOpenFile(const char* fileName, int mode);

/*********************************
 *function: 向文件写入数据
 *return: 实际写入字符个数
 *@para fd: 文件指针
 *@para data: 写入的数据
**********************************/
int VFS_KWriteFile(struct file *fd, char *data);


/*********************************
 *function: 读取文件数据
 *return: 实际读取的字符个数
 *@para fd: 文件指针
 *@para data: 存储读取的数据
 *@para size: data字符串的长度
*********************************/
int VFS_KReadFile(struct file *fd, char *data, size_t size);


/**********************************
 *function: 从文件中读取一行内容
 *return: -1 = 读取成功　　　0 = 文件末尾
 *@para fd: 文件指针
 *@para data: 存储读取的数据
***********************************/
int VFS_KReadLine(struct file *fd, char *data);


/*********************************
 * function: 关闭打开的文件
 * return: 0 = 成功
 * @oara fd: 要关闭文件的指针
*********************************/
int VFS_KCloseFile(struct file *fd);

/**********************************
 * func: 十进制转十六进制
 * return: 返回存放最后一个字符的下一个地址(用户递归计算)
 * @para ch: 存放转化后的十六进制
 * @para num: 要被转化的十进制数
***********************************/
char* decTohex(char *ch, int num);

/****************************************
 * func: 从/proc/net/tcp文件或者udp文件中获取一行数据中的十六进制端口号和对应的inode
 * return: -1 = 没有端口信息   >0 = 获取了这行数据中端口对应的inode号
 * @para str: 一行的数据
 * @para hexPort: 存放十六进制的端口号
****************************************/
int getPort(char *str, char *hexPort);

/****************************************
 * func: 从/proc/net/tcp或者udp文件中获取要查找的端口对应的inode
 * return: -1 = 没有找到    >0 = 对应的inode
 * @para path: 要查找的文件(/proc/net/tcp)
 * @para hex: 要查找的十六进制端口
****************************************/
int judge(char *path, char *hex);

/*************************************
 * func: 通过十进制端口号获取其对应的inode
 * return: -1 = 失败　　>0 = inode
 * @para port: 十进制端口号
*************************************/
int getConflictInode(int port);

/**************************************
 * func: 判断buf与inode是否相同
 * return: true = 相同　　　false = 不相同
 * @para buf: 被判断的字符串
 * @para inode: 被判断的整数
**************************************/
bool judgeConflictID(char *buf, int inode);

/***********************************************
 * func: 获取占用port的进程信息
 * return: 返回查找到的进程信息 如何pid = -1 则表示没有找到对应的冲突信息
 * @para port: 被占用的端口
************************************************/
struct conflictProcess getConflictProcess(int port);

/*****************************************
 * func: 打开文件夹
 * return: -1 = 打开失败　　>0 =　文件夹句柄
 * @para path: 被打开的文件夹路径
*****************************************/
extern int vfs_opendir(const char *path);

/*****************************************************
 * func: 读取文件夹的内容
 * return: 返回文件夹的内容(线性列表)
 * @para fd: 打开的文件夹句柄
*****************************************************/
extern struct KCode_dirent* vfs_readdir(const int fd);

/**************************************************
 * func: 释放存放文件夹内容的列表
 * return: void
 * @para dir: 存放内容的首地址
**************************************************/
extern void vfs_free_readdir(struct KCode_dirent *dir);

/**********************************
 * func: 关闭打开的文件夹
 * return: void
 * @para fd: 打开的文件夹句柄
**********************************/
extern void vfs_closedir(const int fd);

/*****************************************
 * func: 读取一个链接
 * return: sys_readlink系统调用的返回值
 * @para path: 链接的路径
 * @para buf: 存放链接中的内容
 * @para bufsize: buf的大小
******************************************/
extern int vfs_readlink(const char *path, char *buf, int bufsize);

/*****************************************
 * func: 创建一个套接字
 * return: >0 = sockfd   -1 = 失败
 * @para family: 协议族
 * @para type: 类型
 * @para protocol: 协议类型
*****************************************/
extern long vfs_socket(int family, int type, int protocol);

/*****************************************
 * func: 关闭一个套接字
 * return: 0 = 成功　　-1 = 失败
 * @para sockfd: 套接字句柄
*****************************************/
extern long vfs_socketClose(unsigned int sockfd);

/*****************************************
 * func: 对设备的IO管理
 * return: 0 = 成功　　!0 = 失败
 * @para fd: 打开的设备的fd
 * @para cmd: 对设备的控制命令
 * @para arg: 补充参数
*****************************************/
extern long vfs_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg);

#endif
