/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-06 00:44
* Last modified: 2017-08-16 05:32
* Email        : 374648064@qq.com
* Filename     : config.h
* Descrip:q
* function  : 全局的宏定义
******************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <linux/stddef.h>
#include <linux/mutex.h>
#include <linux/types.h>

#define OPENLOG 1    //1: 表示打开日志功能　　　0: 表示关闭日志功能

//log file name
#define ERROR_LOG_FILE    "errorInfo.log"
#define WARNING_LOG_FILE  "warningInfo.log"
#define RESULT_LOG_FILE   "resultInfo.log"

#define MAX_PORT_NUM  200   //per program has max port number

/*
 * 1: automatic monitor all user layer app
 * 0: monitor user specify app
 */
#define MONITOR_TYPE    1

#define SHOWINFO   0     //0: 表示不在终端显示日志   1: 表示在终端显示

#define FILE_PATH_MAX_LENGTH   256    //一个文件的路径最大长度

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数

#define MAX_PROCPATH          30    //proc目录下进程的最大路径  例如/proc/1024

#define CALC_CPU_TIME         1000    //计算CPU时间的间隔, 单位为ms

//定义Netlink的类型
#define NETLINK_USER_MSG  23

//定义存放冲突信息的最大字符个数
#define MAX_CONFLICTINFO      256

//定义ResourceMonitor-Server配置文件存放的路径
#define ResourceMonitor_Server_CONFIG_PATH     "/etc/ResourceMonitorServer.conf"
#define ResourceMonitor_Server_CONFIG_NOTESYMBOL    '#'
#define MONITOR_LABEL         "monitor"  //用于标识监控软件的label
#define MONITOR_KEY           "software" //用于标识监控软件的key

#define MAX_RECORD_LENGTH          5     //during MAX_RECORD_LENGTH*CALC_CPU_TIME ms, Process Resource utilization
#define MAX_CHILD_PROCESS_NUM      21    //A program has the maximum number of processes(MAX_CHILD_PROCESS_NUM-1)

#define CONFIG_KEY_MAX_NUM       50     //配置项key的最大值
#define CONFIG_VALUE_MAX_NUM     30    //配置项value的最大值

#define MAX_SUBSTR               512   //拆分后子字符串的最大长度
#define MAX_INFOLENGTH           50    //获取应用程序占用系统每个资源数据的最大长度

#define HEX_MAX_NUM              15    //存放十六进制的最大字符串长度
#define LINK_MAX_NUM             256   //一个链接信息的最大长度

#define MAX_DIRNAME_LENGTH       64   //存放一个文件夹名称的最大长度
#define MAX_NETCARDNAME_LENGTH   32   //网卡的名称最长字符个数

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
 * function: record process's resource utilization
 * @para name: process's name
 * @para pgid: process group id
 * @para flag: mark process whether exist or not(true = existence   false = non-existence)
 * @para processNum: A program has processNum process
 * @para cpuUsed: record process CPU utilization
 * @para memUsed: record process memory utilization
 * @para schedInfo: record process sched infomation
 * @para ioDataBytes: record process io read and write bytes
 * @para netTotalBytes: record process network send receive bytes
***********************************/
typedef struct ResourceUtilization
{
	char name[MAX_INFOLENGTH];
	pid_t pgid;
	bool flags;
	int processNum;
	int cpuUsed[MAX_RECORD_LENGTH];
	int memUsed[MAX_RECORD_LENGTH];
	int swapUsed[MAX_RECORD_LENGTH];
	unsigned long maj_flt[MAX_RECORD_LENGTH];
	ProcSchedInfo schedInfo[MAX_RECORD_LENGTH];
	unsigned long long ioDataBytes[MAX_RECORD_LENGTH];
	unsigned long long netTotalBytes[MAX_RECORD_LENGTH];
	struct ResourceUtilization *pre;
	struct ResourceUtilization *next;
} ResUtilization;

/************************************
 * function: process resource
 * @para flag: mark process whether exist or not(true = existence   false = non-existence)
************************************/ 
typedef struct ProcessResource
{
	char name[MAX_INFOLENGTH];
	pid_t pgid;
	bool flags;
	int processNum;
	unsigned long VmRss;
	unsigned long swap;
	unsigned long maj_flt;
	unsigned long cpuTime;
	ProcSchedInfo schedInfo;
	unsigned long long ioDataBytes;
	unsigned long long netTotalBytes;
} ProcRes;

/*******************************************
 * function: 存放磁盘的使用状态
********************************************/
typedef struct DiskStats
{
	unsigned long long rd_ios;   //读完成次数
	unsigned long long rd_merges;     //合并读完成次数
	unsigned long long rd_sectors;    //读扇区的次数
	unsigned long long rd_ticks;      //读花费的毫秒数
	unsigned long long wr_ios;        //写完成次数
	unsigned long long wr_merges;     //合并写完成次数
	unsigned long long wr_sectors;    //写扇区的次数
	unsigned long long wr_ticks;      //写花费的毫秒数
	unsigned long long handle_rdwr_num;    //正在处理的输入/输出的请求数
	unsigned long long ticks;      //输入/输出操作花费的毫秒数
	unsigned long long aveq;       //输入/输出的操作花费的加权毫秒数
} DiskStat;

/*******************************************
 * function: 存放每个磁盘的使用状态
*******************************************/
typedef struct DiskStateInfo
{
	char diskName[MAX_DIRNAME_LENGTH];
	DiskStat diskInfo;
	struct DiskStateInfo *next;
} DiskInfo;

/*******************************************
 * function: 记录每个磁盘的使用率情况
*******************************************/
typedef struct DiskUsedInfo
{
	char diskName[MAX_DIRNAME_LENGTH];
	int ioUsed;
	struct DiskUsedInfo *next;
} IOUsedInfo;

/******************************************
 * function: 记录每个网卡的使用情况
******************************************/
typedef struct NetWorkUsedInfo
{
	char netCardName[MAX_NETCARDNAME_LENGTH];   //网卡名称
	int netUsed;
	struct NetWorkUsedInfo *next;
} NetUsedInfo;

/************************************
 * function: 存放系统的总资源使用情况
************************************/
typedef struct SystemResource
{
	int cpuUsed;    //系统的CPU使用率
	int memUsed;    //系统的mem使用率
	int swapUsed;   
	IOUsedInfo *ioUsed;     //系统的io使用率(在统计时间内所有处理io时间除以总共统计时间)
	//以下都是针对单位时间内的变化，不会累计
	NetUsedInfo *netUsed;   //系统的网卡使用率
} SysResource;

/**************************
 * function: 定义/proc文件系统下的进程路径
**************************/
typedef struct ProcPID
{
	char path[MAX_PROCPATH];
	pid_t pid;
	struct ProcPID *next;
} ProcPIDPath;

/**********************************
 * function: 获取每个网卡的流量数据
**********************************/
typedef struct NetCardStats
{
	unsigned long long uploadPackage;     //系统的下载的数据包个数
	unsigned long long downloadPackage;   //系统的上传的数据包个数
	unsigned long long uploadBytes;    //系统的上传字节
	unsigned long long downloadBytes;  //系统的下载字节
} NetCardStat;

/************************************
 * function: 定义系统网络相关的信息(/proc/net/dev)  注意: 是系统中所有网卡的数据和
************************************/
typedef struct NetStateInfo
{
	char netCardName[MAX_NETCARDNAME_LENGTH];   //网卡名称
	NetCardStat netCardInfo;
	struct NetStateInfo *next;
} NetInfo;

/**************************
 * function: 定义物理内存相关的信息(/proc/meminfo)
**************************/
typedef struct TotalMemInfo
{
	unsigned long memTotal;      //物理总内存大小
	unsigned long memAvailable;  //可以物理内存大小
	unsigned long totalswap;
	unsigned long freeswap;
} MemInfo;

/***************************
 * function: 定义CPU使用时间
 * 详细信息查看 man proc
***************************/
typedef struct TotalCpuTime
{
	unsigned long long user;
	unsigned long long nice;
	unsigned long long system;
	unsigned long long idle;
} Total_Cpu_Occupy_t;

/***************************
 * function: 定义程序CPU使用时间
 * 详细信息查看 man proc
****************************/
typedef struct ProcessCpuTime
{
	int pid;
	unsigned long utime;
	unsigned long stime;
	unsigned long cutime;
	unsigned long cstime;
} Process_Cpu_Occupy_t;

/***************************
 * function: process memory info
 * @para rss: physical memory
 * @para swap: virual memory
***************************/
typedef struct ProcessMemInfo
{
	unsigned long rss;
	unsigned long swap;
} Process_Mem_Info;

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
 * inDataBytes: 该端口对应的下载数据字节
 * outDataBytes: 该端口对应的上传数据字节
 * next: 下一个端口数据
****************************/
typedef struct PortMapPackage
{
	int port;
	char protocol;
	unsigned int inPackageSize;
	unsigned int outPackageSize;
	unsigned long long inDataBytes;
	unsigned long long outDataBytes;
	struct PortMapPackage *next;
} Port_Map_Package;

extern Port_Map_Package *PortPackageData;         //存放当前时间段每个端口所发送所接受的数据包信息
extern Port_Map_Package *beginPortPackageData;    //PortPackageData 第一个元素
extern Port_Map_Package *endPortPackageData;      //PortPackageData 最后一个元素
extern Port_Map_Package *currentPortPackageData;  //PortPackageData 当前操作的元素

//每一位代表一种资源是否冲突   1 = 冲突   0 = 不冲突
#define CPU_CONFLICT    1    //CPU资源冲突(00000001)
#define MEM_CONFLICT    2    //MEM资源冲突(00000010)
#define NET_CONFLICT    4    //NET资源冲突(00000100)
#define IO_CONFLICT     8    //IO资源冲突 (00001000)
#define PORT_CONFLICT   16   //端口资源冲突(00010000)

/**********************************
 * function: 存放资源冲突的进程信息
 * name: conflict process name
 * pgid: process group id
 * conflictType: 冲突的类型
 * conflictInfo: 存放冲突的信息
 * next: 下一个地址
**********************************/
typedef struct ConflictProcess
{
	char name[MAX_INFOLENGTH];
	pid_t pgid;
	int conflictType;
	char conflictInfo[MAX_CONFLICTINFO];
	struct ConflictProcess *next;
} ConflictProcInfo;


extern ConflictProcInfo *beginConflictProcess;   //冲突信息的头
extern ConflictProcInfo *endConflictProcess;     //冲突信息的尾
extern ConflictProcInfo *currentConflictProcess; //当前的冲突信息

//冲突信息互斥锁
extern struct mutex ConflictProcess_Mutex;

//系统资源使用率临界值
extern int SYS_MAX_CPU;
extern int SYS_MAX_MEM;
extern int SYS_MAX_SWAP;
extern int SYS_MAX_IO;
extern int SYS_MAX_NET;

/*
 * the max value of process resource unilization
 * PROC_MAX_SCHED: the max value of process sched
 */
extern int PROC_MAX_CPU;
extern int PROC_MAX_MEM;
extern int PROC_MAX_MAJ_FLT;
extern unsigned long long PROC_MAX_IO;
extern unsigned long long PROC_MAX_NET;
extern ProcSchedInfo PROC_MAX_SCHED;

/**********************************
 * function: 监控软件的名称列表
 * @para name: program name
 * @para pgid: program group id
**********************************/
typedef struct MonitorProgramName
{
	char name[MAX_INFOLENGTH];
	pid_t pgid;
	struct MonitorProgramName *next;
} MonitorAPPName;

/**********************************
 * function: A program's all pid
 * @para sockflag: whether /proc/pid/fd/ has socket link or not
 * @para usePort: program port inode
 * @para usePort_index: the max number of port inode
**********************************/
typedef struct ProgramAllPid
{
	bool sockflag;
	char name[MAX_INFOLENGTH];
	int usePort[MAX_PORT_NUM];
	int usePort_index;
	pid_t pgid;
	int childPid[MAX_CHILD_PROCESS_NUM];
	struct ProgramAllPid *next;
} ProgAllPid;

/*********************************
 * function: per process user resource info
 * @para flags: mark current process whether validity or not
 *				true = valid
 *				false = invalid
**********************************/
typedef struct ProgramAllRes
{
	char name[MAX_INFOLENGTH];
	bool flags[MAX_CHILD_PROCESS_NUM];
	pid_t pgid;
	unsigned long cpuTime[MAX_CHILD_PROCESS_NUM];
	unsigned long maj_flt[MAX_CHILD_PROCESS_NUM];
	unsigned long long ioDataBytes[MAX_CHILD_PROCESS_NUM];
	ProcSchedInfo schedInfo[MAX_CHILD_PROCESS_NUM];
} ProgAllRes;

//system resouces unilization list
extern SysResource sysResArray[MAX_RECORD_LENGTH];

//record current system resource unilization index
extern int currentRecordSysResIndex;

//用户层的APP列表
extern MonitorAPPName *beginMonitorAPPName;     //用户层APP列表头
extern MonitorAPPName *endMonitorAPPName;       //用户层APP列表尾
extern MonitorAPPName *currentMonitorAPPName;   //当前操作的用户层APP

//系统用户层APP个数
extern int MonitorAPPNameNum;

extern ResUtilization *beginMonitorAPP;
extern ResUtilization *endMonitorAPP;
extern ResUtilization *currentMonitorAPP;

//record current operate ResUtilization array index
extern int currentRecordResIndex;

extern ProgAllPid *beginMonitorProgPid;
extern ProgAllPid *endMonitorProgPid;
extern ProgAllPid *currentMonitorProgPid;

extern IOUsedInfo *beginDiskUsedInfo;
extern IOUsedInfo *currentDiskUsedInfo;
extern IOUsedInfo *tailDiskUsedInfo;
extern int currentDiskNum;

extern NetUsedInfo *beginNetUsedInfo;
extern NetUsedInfo *currentNetUsedInfo;
extern NetUsedInfo *tailNetUsedInfo;
extern int currentNetNum;

extern int avgSYSCpuUsed;
extern int avgSYSMemUsed;
extern int avgSYSSwapUsed;

#endif

