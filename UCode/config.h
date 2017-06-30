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

#define MAX_INFOLENGTH           50    //获取应用程序占用系统每个资源数据的最大长度

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
 * function: 存放进程使用的系统资源数据
***********************************/
typedef struct ProcessInfo
{
	char name[MAX_INFOLENGTH];   //进程的名称
	int pid;
	int ppid;
	int cpuUsed;      //CPU使用率
	int memUsed;      //MEM使用率
	ProcSchedInfo schedInfo;
	char VmPeak[MAX_INFOLENGTH];   //进程虚拟内存大小(单位: KB)
	char VmRSS[MAX_INFOLENGTH];    //进程物理内存大小(单位: KB)
	char State[MAX_INFOLENGTH];    //进程状态
	unsigned long long ioSyscallNum;    //read/pread和write/pwrite系统调用次数
	unsigned long long ioDataBytes;     //read_write_bytes大小
	int uploadPackage;   //上传数据包个数
	int downloadPackage; //下载数据包个数
	int totalPackage;    //上传数据包+下载数据包
	unsigned long long uploadBytes;   //上传字节数
	unsigned long long downloadBytes;   //下载字节数
	unsigned long long totalBytes;      //上传字节数+下载字节数
} ProcInfo;

/**********************************
 * function: 存放资源冲突的进程信息
 * name: process name
 * conflictType: 冲突的类型
 * next: 下一个地址
**********************************/
typedef struct ConflictProcess
{
	char name[MAX_INFOLENGTH];   //进程的名称
	int conflictType;
	struct ConflictProcess *next;
} ConflictProcInfo;

//每一位代表一种资源是否冲突   1 = 冲突   0 = 不冲突
#define CPU_CONFLICT    1    //CPU资源冲突(00000001)
#define MEM_CONFLICT    2    //MEM资源冲突(00000010)
#define NET_CONFLICT    4    //NET资源冲突(00000100)
#define IO_CONFLICT     8    //IO资源冲突 (00001000)
#define PORT_CONFLICT   10   //端口资源冲突(00010000)

#endif
