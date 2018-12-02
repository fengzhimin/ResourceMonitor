/******************************************************
* Author       : fengzhimin
* Create       : 2018-11-05 05:30
* Last modified: 2018-11-05 05:30
* Email        : 374648064@qq.com
* Filename     : config.c
* Description  : 用于定义全局变量
******************************************************/

#include "config.h"

ConflictProcInfo *beginConflictProcess = NULL;   //冲突信息的头
ConflictProcInfo *endConflictProcess = NULL;     //冲突信息的尾

int skfd;
struct nlmsghdr *nlh = NULL;
struct termios newTermios;
struct termios oldTermios;
pthread_mutex_t showOtherInfo_mutex;
pthread_mutex_t conflictProcess_mutex; 
bool showOtherInfo = false;   //标记显示版本信息或者帮助信息
int conflictCount = 0;
char label[CONFIG_LABEL_MAX_NUM] = {0};  //配置文件中的标签
char name[CONFIG_VALUE_MAX_NUM] = {0};   //配置文件中的配置项名称
char increaseValue[CONFIG_VALUE_MAX_NUM] = {0};  //配置文件中的每次增加配置的量
char defaultValue[CONFIG_VALUE_MAX_NUM] = {0};   //配置文件中的配置项默认值
char softwareName[SOFTWARE_NAME_MAX_NUM] = {0};  //软件的名称

