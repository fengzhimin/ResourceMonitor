/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-04 10:23
* Last modified: 2017-03-17 16:52
* Email        : 374648064@qq.com
* Filename     : fileOper.h
* Description  : 文件操作的头文件
******************************************************/

#ifndef __FILEOPER_H__
#define __FILEOPER_H__

#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define KReadLine(fd, data) KReadLineDebug(fd, data, __FILE__, __FUNCTION__, __LINE__)

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
int KReadLineDebug(struct file *fd, char *data, const char *file, const char *function, const int line);


/*********************************
 * function: 关闭打开的文件
 * return: 0 = 成功
 * @oara fd: 要关闭文件的指针
*********************************/
int KCloseFile(struct file *fd);


/******************************
 * function: 过滤注释行
 * return: void
 * @para fileName：要被过滤的文件名
 * @para fileNameCopy：过滤以后的数据存放的文件 
******************************/
void RemoveNote(char *fileName, char *fileNameCopy);

/*********************************
 * function: judge whether file is empty
 * return: 0 = empty   1 = non-empty   -1 = read file failure
 * @para fileName: a file path
*********************************/
int IsEmpty(char *fileName);

#endif
