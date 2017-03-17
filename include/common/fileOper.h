/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-04 10:23
* Last modified: 2016-11-12 01:06
* Email        : 374648064@qq.com
* Filename     : fileOper.h
* Description  : 文件操作的头文件
******************************************************/

#ifndef __FILEOPER_H__
#define __FILEOPER_H__

#include <linux/string.h>

/**********************************
 * function: 打开文件操作
 * return: fd=成功  NULL=失败并且设置errno值 通过strerror函数来获取具体错误内容
 * @para fileName: 打开的文件名
 * @para  mode: 打开文件的模式
 *        mode: r  只读方式打开文件
	      mode: r+ 读写方式打开，将文件指针定位到文件头
	      mode: w  只写方式打开
		  mode: w+ 读写方式打开，如果文件不存在则创建新文件，将文件指针定位到文件头
		  mode: a  追加方法打开，如果文件不存在则创建新文件，将文件指针定位到文件末尾
		  mode: a+ 追加方式打开，可以读写操作。
**********************************/
FILE *OpenFile(const char* fileName, const char* mode);


/*********************************
 *function: 向文件写入数据
 *return: -1 = 成功  0 = EOF   errno = 失败　通过strerror函数来获取具体的错误内容
 *@para fd: 文件指针
 *@para data: 写入的数据
**********************************/
int WriteFile(FILE *fd, char *data);


/*********************************
 *function: 读取文件数据
 *return: -1 = 成功 0 = EOF  errno = 失败 通过strerror函数来获取具体的错误内容
 *@para fd: 文件指针
 *@para data: 存储读取的数据
 *@para size: data字符串的长度
*********************************/
int ReadFile(FILE *fd, char *data, size_t size);


/**********************************
 *function: 从文件中读取一行内容
 *return: -1 = 读取成功　　　0 = 文件末尾
 *@para fd: 文件指针
 *@para data: 存储读取的数据
***********************************/
int ReadLine(FILE *fd, char *data);


/*********************************
 * function: 关闭打开的文件
 * return: 0 = 成功  EOF = 失败并且设置errno值　通过strerror函数来获取具体的错误内容
 * @oara fd: 要关闭文件的Id号
*********************************/
int CloseFile(FILE *fd);


/******************************
 * function: 过滤注释行
 * return: void
 * @para fileName：要被过滤的文件名
 * @para fileNameCopy：过滤以后的数据存放的文件 
******************************/
void RemoveNote(char *fileName, char *fileNameCopy);

#endif
