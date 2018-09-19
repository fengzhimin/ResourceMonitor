/******************************************************
* Author       : fengzhimin
* Create       : 2018-09-18 19:23
* Last modified: 2018-09-18 19:23
* Email        : 374648064@qq.com
* Filename     : ioOper.h
* Description  : common io operation
******************************************************/

#ifndef __IOOPER_H__
#define __IOOPER_H__

#include <sys/select.h>
#include <sys/types.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

/***********************************
 * func: read characters through non-blocking mode
 * return: no-input = 0 otherwise return key code
***********************************/
char getch();

#endif
