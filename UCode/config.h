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

/***********************************
 * function: 存放进程使用的系统资源数据
***********************************/
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
	unsigned long long uploadBytes;   //上传字节数
	unsigned long long downloadBytes;   //下载字节数
	unsigned long long totalBytes;      //上传字节数+下载字节数
} ProcInfo;

/**********************************
 * function: 存放资源冲突的进程信息
 * processInfo: 进程的资源使用情况
 * conflictType: 冲突的类型
 * next: 下一个地址
**********************************/
typedef struct ConflictProcess
{
	ProcInfo processInfo;
	int conflictType;
	struct ConflictProcess *next;
} ConflictProcInfo;

#endif
