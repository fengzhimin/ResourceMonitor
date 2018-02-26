/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-06 15:27
* Last modified: 2016-11-07 23:34
* Email        : 374648064@qq.com
* Filename     : dirOper.h
* Description  : 文件夹操作函数头文件
******************************************************/
#include "config.h"
#include <linux/fs.h>
#include <asm/uaccess.h>

/***************************************
 *function: 判断是否为一个文件夹
 *return: -3 = 是其他类型文件   -2 = 是一个普通文件　　-1 = 是一个文件夹  0 = 失败
 *@para path: 要判断的文件名称(包括路径)
****************************************/
int Is_Dir(const char *path);
