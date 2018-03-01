/******************************************************
* Author       : fengzhimin
* Email        : 374648064@qq.com
* Filename     : fileOper.h
* Descripe     : common file operation
******************************************************/

#ifndef __FILEOPER_H__
#define __FILEOPER_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "config.h"

/**********************************
 * function: 打开文件操作
 * return: struct >=0 成功  -1=失败
 * @para fileName: 打开的文件名
 * @para  mode: 打开文件的模式
 *		  mode: O_RDONLY 只读方式打开
 *		  mode: O_WRONLY 只写方式打开
 *		  mode: O_RDWR 读写方式打开
 *		  mode: O_APPEND 追加方式打开
**********************************/
int OpenFile(const char* fileName, int mode);

/*********************************
 *function: 向文件写入数据
 *return: 实际写入字符个数
 *@para fd: 文件指针
 *@para data: 写入的数据
**********************************/
int WriteFile(int fd, char *data);


/*********************************
 *function: 读取文件数据
 *return: 实际读取的字符个数
 *@para fd: 文件指针
 *@para data: 存储读取的数据
 *@para size: data字符串的长度
*********************************/
int ReadFile(int fd, char *data, size_t size);


/**********************************
 *function: 从文件中读取一行内容
 *return: -1 = 读取成功　　　0 = 文件末尾
 *@para fd: 文件指针
 *@para data: 存储读取的数据
***********************************/
int ReadLine(int fd, char *data);

/*********************************
 * function: 关闭打开的文件
 * return: 0 = 成功
 * @oara fd: 要关闭文件的指针
*********************************/
int CloseFile(int fd);

#endif