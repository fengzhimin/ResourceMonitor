/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-09 11:40
* Last modified: 2017-03-19 20:50
* Email        : 374648064@qq.com
* Filename     : strOper.h
* Description  : 字符串处理函数
******************************************************/

#ifndef __STROPER_H__
#define __STROPER_H__

#include <linux/string.h>
#include <linux/slab.h>
#include "config.h"

/**************************
 * function: 判断一个字符串是否为数字
 * return: true = 是数字　false = 不是一个数字
 * @para _ch: 要判断的字符串
**************************/
bool isNum(char *_ch);


/*************************
 * function: 获取字符串中第一个非空字符的下标
 * return: 返回下标值
 * @para _str: 要判断的字符串
*************************/
int GetFirstCharIndex(char *_str);

/**************************
 * function: 判断一行是否为注释行
 * return: true = 是注释行　　false = 不是注释行
 * @para _str: 要判断的行字符串
***************************/
bool JudgeNote(char *_str);


/**************************
 * function: 获取字符串中第一个单词
 * return: -1 = 提取失败　>0 = 第一个单词的下一个index
 * @para _str: 要截取的字符串
 * @para _first_word: 存储截取后的第一次单词
***************************/
int GetFirstWordFromStr(char *_str, char *_first_word);


/**************************
 * function: 判断一个字符串是否为配置项　　这里的配置项只针对 key = value 和　key  value型两种
 * return: true = 是符合要求的配置项    false = 不是符合要求的配置项
 * @para _str: 要判断的字符串
 * @para _type: 配置项类型
 * @para _type_num: 配置项类型个数
 * @para _configInfo: 存储配置项信息的指针(key, value)
***************************/
bool GetConfigInfo(char *_str, char _type[][CONFIG_KEY_MAX_NUM], int _type_num, ConfigInfo *_configInfo);


/*********************************
 * function: 获取配置文件中的配置项个数
 * return: -1 = 获取失败　　>0 = 配置项个数
 * @para _configInfo: 配置项的信息
 * @para _type: 配置项的类型
 * @para _type_num: 存储配置项类型数组的大小
 * @para _configfilepath: 配置文件数组
 * @para _configfilepathNum: 配置文件个数
**********************************/
int GetConfigInfoFromConfigFile(ConfigInfo _configInfo[], char _type[][CONFIG_KEY_MAX_NUM], int _type_num, char _configfilepath[][FILE_PATH_MAX_LENGTH], int _configfilepathNum);


/**********************************
 * function: 提取字符串中的数字(只提取正整数)
 * return: -1 = 没有数字  >=0 = 返回提取的数字
 * @para _str: 要被提取的字符串
***********************************/
unsigned long long ExtractNumFromStr(char *_str);


/**********************************
 * func: 通过字符ch来拆分字符串
 * return: 1 = 不可拆分   拆分后子字符串的个数 > 1
 * @para str: 要被拆分的字符串
 * @para ch: 分隔字符
 * @para subStr: 存放拆分后的子字符串的数组
 * @para subStrLength: subStr的大小
 * example: str = str1:str2:str3  --->  ch = ':'时 subStr = str1 str2 str3
***********************************/
int cutStrByLabel(char *str, char ch, char subStr[][MAX_SUBSTR], int subStrLength);


/**********************************
 * func: 去掉字符串中开头的所有的空格和制表符
 * return: void
 * @para str: 要被处理的字符串数组
 * example: str = "  str"    ---->    str = "str"
***********************************/
void removeBeginSpace(char *str);

/**********************************
 * func: 移除字符串中所有的指定字符
 * return: void
 * @para str: 处理的字符串数组
 * @para ch: 要被移除的字符
 * example: str = "1 2 3" ch = ' '  ------>  str = "123"
***********************************/
void removeChar(char *str, char ch);

/**********************************
 * func: 获取字符串中子字符串的个数
 * return: int
 * @para str: 原字符串
 * @para substr: 要查找的字符串
 * example: str = "str123str23" substr = "str"  -----> return = 2
***********************************/
int GetSubStrNum(char *str, char *substr);

/**********************************
 * func: 将整数转化为字符串
 * return: 存放最后一个字符的下一个字符的地址
 * @para str: 存放转化后的字符串
 * @para num: 整数
 * example: num = 12 -------> str = "12"
***********************************/
char* IntToStr(char *str, int num);

/**********************************
 * func: 将字符串整数转化为int整数(正数)
 * return: 转化后的int
 * @para str: 存放转化前的字符串
 * example: str = "12" -------> return = 12
***********************************/
int StrToInt(char *str);

#endif
