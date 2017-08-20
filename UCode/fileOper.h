/******************************************************
* Author       : fengzhimin
* Create       : 2017-08-19 06:54
* Last modified: 2017-08-19 06:54
* Email        : 374648064@qq.com
* Filename     : fileOper.h
* Description  : 
******************************************************/
#ifndef __FILEOPER_H__
#define __FILEOPER_H__

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

/****************************************************
 * func: open a file
 * return: -1 = error   >0 = file descriptor
 * @para pathname: file path
****************************************************/
int OpenFile(char *pathname);

/****************************************************
 * func: read a line data from file
 * return: false = error   true = success
 * @para fd: file descriptor
 * @para data: save data
*****************************************************/
bool readline(int fd, char *data);

/****************************************************
 * func: close a opened file
 * return: false = error   true = success
 * @para fd: file descriptor
****************************************************/
bool CloseFile(int fd);

#endif
