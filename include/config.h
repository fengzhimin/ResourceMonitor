/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-06 00:44
* Last modified: 2017-04-10 18:47
* Email        : 374648064@qq.com
* Filename     : config.h
* Descrip:q
* function  : 全局的宏定义
******************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <linux/stddef.h>

#define OPENLOG 1    //1: 表示打开日志功能　　　0: 表示关闭日志功能

#define CONFIG_FILE_MAX_NUM   100   //一个软件的最大配置文件个数由程序自动寻找配置文件

#define FILE_PATH_MAX_LENGTH   256    //一个文件的路径最大长度

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数

#define MAX_FUNCNAME          50    //函数名称的最大字符个数

#define MAX_PROCPATH          30    //proc目录下进程的最大路径  例如/proc/1024

#define CALC_CPU_TIME         1000    //计算CPU时间的间隔, 单位为ms

//当进程的CPU使用率大于PROCESSRELATECPUDOWN小于PROCESSRELATECPUUP时,计算父进程和父父进程的资源使用情况
#define PROCESSRELATECPUDOWN      10   
#define PROCESSRELATECPUUP        20

//当进程的MEM使用率大于PROCESSRELATEMEMDOWN小于PROCESSRELATEMEMUP时,计算父进程和父父进程的资源使用情况
#define PROCESSRELATEMEMDOWN      20   
#define PROCESSRELATEMEMUP        30

extern char config_type[][20];    //配置文件的类型

/*****************************
 * function: 获取config_type的个数
 * return: 个数
******************************/
int GetConfig_TypeNum(void);

extern char note_symbol[][10];    //注释符号

/*****************************
 * function: 获取note_symbol的个数
 * return:　个数
******************************/
int GetNote_SymbolNum(void);

#define CONFIG_KEY_MAX_NUM       50     //配置项key的最大值
#define CONFIG_VALUE_MAX_NUM     30    //配置项value的最大值

#define MAX_SUBSTR               256   //拆分后子字符串的最大长度
#define MAX_INFOLENGTH           50    //获取应用程序占用系统每个资源数据的最大长度

#define HEX_MAX_NUM              15    //存放十六进制的最大字符串长度
#define LINK_MAX_NUM             256   //一个链接信息的最大长度


typedef struct ProcessInfo
{
	char name[MAX_INFOLENGTH];   //进程的名称
	int pid;
	int ppid;
	int cpuUsed;   //CPU使用率
	int memUsed;   //MEM使用率
	char VmPeak[MAX_INFOLENGTH];   //进程虚拟内存大小(单位: KB)
	char VmRSS[MAX_INFOLENGTH];    //进程物理内存大小(单位: KB)
	char State[MAX_INFOLENGTH];    //进程状态
	unsigned long long ioSyscallNum;    //read/pread和write/pwrite系统调用次数
	unsigned long long ioDataBytes;     //read_write_bytes大小
	int uploadPackage;   //上传数据包个数
	int downloadPackage; //下载数据包个数
	int totalPackage;    //上传数据包+下载数据包
} ProcInfo;

typedef struct SystemResource
{
	int cpuUsed;    //系统的CPU使用率
	int memUsed;    //系统的mem使用率
} SysResource;

/**************************
 * function: 定义/proc文件系统下的进程路径
**************************/
typedef struct ProcPID
{
	char path[MAX_PROCPATH];
	struct ProcPID *next;
} ProcPIDPath;

/***************************
 * function: 定义配置项结构体
***************************/
typedef struct ConfigInfo
{
	char key[CONFIG_KEY_MAX_NUM];
	char value[CONFIG_VALUE_MAX_NUM];
} ConfigInfo;

/***************************
 * function: 定义CPU使用时间
 * 详细信息查看 man proc
***************************/
typedef struct TotalCpuTime
{
	unsigned int user;
	unsigned int nice;
	unsigned int system;
	unsigned int idle;
} Total_Cpu_Occupy_t;

/***************************
 * function: 定义程序CPU使用时间
 * 详细信息查看 man proc
****************************/
typedef struct ProcessCpuTime
{
	int pid;
	unsigned int utime;
	unsigned int stime;
	unsigned int cutime;
	unsigned int cstime;
} Process_Cpu_Occupy_t;

/**************************
 * function: 定义进程IO数据
 * 详细信息查看 man proc
**************************/
typedef struct ProcessIOData
{
	unsigned long long rchar;
	unsigned long long wchar;
	unsigned long long syscr;
	unsigned long long syscw;
	unsigned long long read_bytes;
	unsigned long long write_bytes;
	unsigned int cancelled_write_bytes;
} Process_IO_Data;


/***************************
 * function: 定义端口与数据包的对应关系
 * port: 端口
 * protocol: 表示数据包是什么类型的协议  'T' =TCP 'U' = UDP
 * inPackageSize: 该端口对应的下载数据包个数
 * outPackageSize: 该端口对应的上传数据包个数
 * next: 下一个端口数据
****************************/
typedef struct PortMapPackage
{
	int port;
	char protocol;
	unsigned int inPackageSize;
	unsigned int outPackageSize;
	struct PortMapPackage *next;
} Port_Map_Package;

extern Port_Map_Package *PortPackageData;         //存放当前时间段每个端口所发送所接受的数据包信息
extern Port_Map_Package *beginPortPackageData;    //PortPackageData 第一个元素
extern Port_Map_Package *endPortPackageData;      //PortPackageData 最后一个元素
extern Port_Map_Package *currentPortPackageData;  //PortPackageData 当前操作的元素

#endif

