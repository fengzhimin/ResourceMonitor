/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-06 15:27
* Last modified: 2016-11-07 23:34
* Email        : 374648064@qq.com
* Filename     : dirOper.h
* Description  : 文件夹操作函数头文件
******************************************************/
#include <stdio.h>
#include "config.h"

/**************************************
 * function: 利用宏定义来设置默认参数
***************************************/
#define AutoFindConfigFile(path, type, configfilepath)  FindFileByType(path, type, configfilepath, 0)

	
/***************************************
 *function: 判断是否为一个文件夹
 *return: -3 = 是其他类型文件   -2 = 是一个普通文件　　-1 = 是一个文件夹  errno = 失败
 *@para path: 要判断的文件名称(包括路径)
****************************************/
int Is_Dir(const char *path);


/***************************************
 *function: 判断一个文件是否为配置文件
 *return: 1 = 匹配　０ = 不匹配
 *@para path:　需要查询的文件的绝对路径
 *@para type:　需要查询的文件类型
 *@para configfilepath: 存储查询的结果
 *@para point:　将匹配的文件路径存储在configfilepath数组下标为point
***************************************/
int JudgeConfFile(char *path, const char type[][20], char configfilepath[][FILE_PATH_MAX_LENGTH], int point);


/***************************************
 *function: 获取一个文件夹下所有的指定文件类型的文件路径和名称
 *return: -1 = 为超过最大配置文件个数  -2 = 输入的不是文件夹　 -3 = 打开文件夹失败    point最大值(>0)也就是配置文件的个数
 *@para path:　需要查询的文件夹的绝对路径
 *@para type:　需要查询的文件类型
 *@para configfilepath: 存储查询的结果
 *@para point:　标记当前已经寻找了point个配置文件 用于函数递归的时候准确的定位到configfilepath数组的第一个空位的下标
***************************************/
int FindFileByType(char *path, const char type[][20], char configfilepath[][FILE_PATH_MAX_LENGTH], int point);
