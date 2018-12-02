/******************************************************
* Author       : fengzhimin
* Create       : 2017-05-08 13:11
* Last modified: 2017-05-08 13:11
* Email        : 374648064@qq.com
* Filename     : config.h
* Description  : 
******************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <linux/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <linux/netlink.h>
#include <pthread.h>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define VERSION "1.0.0"

#define COPYRIGHT "(C) 2017-2018 zhimin feng"

#define PROCNAME  "ResourceMonitor"   //进程名称

#define OPENLOG  1      //0=不记录日志   1=记录日志
#define SHOWINFO   1     //0: 表示不在终端显示日志   1: 表示在终端显示

#define ONLINE_RESOLUTION     //0=不在线资源竞争消解    1=在线资源竞争消解

//确保客户端请求数据的频率大于服务器更新数据的频率(消解冲突的频率)
#define REQUEST_MESSAGE_RATE   1500000    //request message rate from server(us)

//显示冲突信息频率
#define SHOW_MESSAGE_RATE      1000000    //show contention information rate(us)

//增加配置值的频率
#define INCREASE_CONFIG_RATE   2000000    //the rate of increase config(us)

//log file name
#define ERROR_LOG_FILE    "/var/log/ResourceMonitor/Client/errorInfo.log"
#define WARNING_LOG_FILE  "/var/log/ResourceMonitor/Client/warningInfo.log"
#define RESULT_LOG_FILE   "/var/log/ResourceMonitor/Client/resultInfo.log"
//the max size of log file(10M = 1024*1024*10)
#define MAX_LOG_FILE_SIZE  10485670
//#define GET_CONFIG_VALUE_FILE   "/var/log/ResourceMonitor/Client/configValue.txt"

#define MAX_NAMELENGTH           50    //应用程序名称最大字符长度

//定义存放冲突信息的最大字符个数
#define MAX_CONFLICTINFO      256

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数
#define CONFIG_LABEL_MAX_NUM     64     //配置文件中label标签的最大长度
#define CONFIG_KEY_MAX_NUM       64     //配置项key的最大值
#define CONFIG_VALUE_MAX_NUM     1024   //配置项value的最大值
#define SOFTWARE_NAME_MAX_NUM    64     //软件名称最大长度
#define MAX_SUBSTR               1024   //拆分后子字符串的最大长度
//定义ResourceMonitor-Client配置文件存放的路径
#define ResourceMonitor_Client_CONFIG_PATH     "/etc/ResourceMonitor/Client/ResourceMonitorClient.conf"
#define ResourceMonitor_Client_CONFIG_NOTESYMBOL    '#'
//存放修改配置项信息，用于增加后续配置项值
#define REDUCE_CONFIG_PATH       "/tmp/ResourceMonitor_Reduce_Config.info"
#define TMP_REDUCE_CONFIG_PATH       "/tmp/TMP_ResourceMonitor_Reduce_Config.info"
//save the script of contention solution
#define ResourceMonitor_Client_SOLUTION_PATH   "/etc/ResourceMonitor/Client/Resolution"
#define SCRIPT_PATH_MAX_LENGTH    128    // the max length of the script path
#define COMMAND_MAX_LENGTH        1024   // the max length of the command
//存放端口冲突信息的临时文件
#define PORT_CONTENTION_INFO_PATH    "/etc/conflictPortInfo.info"

# define MAX_ARGV_LENGTH       2048   //max argv parameters length
/*************************************
 * function: 存放关于进程调度的时间数值
*************************************/
typedef struct ProcessSchedInfo
{
	int sum_exec_runtime;   //累计运行的物理时间
	int wait_sum;     //在就绪队列里的等待时间
	int iowait_sum;   //io等待时间
} ProcSchedInfo;

/***********************************
 * function: 记录软件执行时的资源使用情况
 * @para cpuUsed: 软件运行时占用CPU情况
 * @para memUsed: 软件运行时占用MEM情况
 * @para swapUsed: 软件运行时占用交换分区的情况
 * @para ioDataBytes: 软件运行时IO读写情况
 * @para netTotalBytes: 软件运行时NET上传下载情况
***********************************/
typedef struct ProcResourceUtilization
{
	int cpuUsed;
	int memUsed;
	int swapUsed;
	unsigned long long ioDataBytes;
	unsigned long long netTotalBytes;
} ProcResUtilization;

/**********************************
 * function: 存放资源冲突的进程信息
 * name: conflict process name
 * pgid: process group id
 * conflictType: 冲突的类型
 * normalResUsed: 正常运行时资源使用情况
 * conflictResUsed: 冲突时资源使用情况
 * conflictInfo: 存放冲突的信息
 * next: 下一个地址
**********************************/
typedef struct ConflictProcess
{
	char name[MAX_NAMELENGTH];
	char conflictType;
	pid_t pgid;
	ProcResUtilization normalResUsed;
	ProcResUtilization conflictResUsed;
	char conflictInfo[MAX_CONFLICTINFO];
	struct ConflictProcess *next;
} ConflictProcInfo;

//每一位代表一种资源是否冲突   1 = 冲突   0 = 不冲突
#define CPU_CONFLICT    1    //CPU资源冲突(00000001)
#define MEM_CONFLICT    2    //MEM资源冲突(00000010)
#define NET_CONFLICT    4    //NET资源冲突(00000100)
#define IO_CONFLICT     8    //IO资源冲突 (00001000)
#define PORT_CONFLICT   16   //端口资源冲突(00010000)

#define NETLINK_USER 22
#define USER_MSG    (NETLINK_USER + 1)
#define MSG_LEN 100
#define DATA_SPACE   100

extern ConflictProcInfo *beginConflictProcess;   //冲突信息的头
extern ConflictProcInfo *endConflictProcess;     //冲突信息的尾

extern int skfd;   //连接服务器的socket描述符
extern struct nlmsghdr *nlh;  //设置netlink的数据
extern struct termios newTermios;   //重新设计新的终端属性
extern struct termios oldTermios;   //保存旧的终端属性
extern pthread_mutex_t showOtherInfo_mutex;   //对显示辅助信息变量(showOtherInfo)进行加锁
extern pthread_mutex_t conflictProcess_mutex;   //对beginConflictProcess、endConflictProcess和currentConflictProcess变量访问加锁
extern bool showOtherInfo;   //标记显示版本信息或者其他帮助信息而非显示竞争信息
extern int conflictCount;  //竞争次数  用于计算竞争发生的时间
extern char label[CONFIG_LABEL_MAX_NUM];  //配置文件中的标签
extern char name[CONFIG_VALUE_MAX_NUM];   //配置文件中的配置项名称
extern char increaseValue[CONFIG_VALUE_MAX_NUM];  //配置文件中的每次增加配置的量
extern char defaultValue[CONFIG_VALUE_MAX_NUM];   //配置文件中的配置项默认值
extern char softwareName[SOFTWARE_NAME_MAX_NUM];  //软件的名称

struct _my_msg
{
    struct nlmsghdr hdr;
	ConflictProcInfo conflictInfo;
};

#endif
