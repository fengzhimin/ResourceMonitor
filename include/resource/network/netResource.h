/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-27 06:38
* Last modified: 2017-05-02 18:37
* Email        : 374648064@qq.com
* Filename     : netResource.h
* Description  : 
******************************************************/

#ifndef __NETRESOURCE_H__
#define __NETRESOURCE_H__

#include "config.h"
#include "common/fileOper.h"
#include "common/strOper.h"
#include "log/logOper.h"
#include <linux/netdevice.h>
#include <linux/filter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/vmalloc.h>
#include <linux/monitorResource.h>

/****************************************
 * func: 截取数据包函数，对数据包进行分析
 * return: int
 * @para type = 标识数据包是那个阶段的数据包
 * @para psbk = 截取的数据包
*****************************************/
unsigned int filter_http(char *type, struct sk_buff *pskb);

/****************************************
 * func: 处理LocalIn阶段的钩子函数
****************************************/
unsigned int NET_HookLocalIn(unsigned int hook, struct sk_buff *pskb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*));

/****************************************
 * func: 处理LocalOut阶段的钩子函数
****************************************/
unsigned int NET_HookLocalOut(unsigned int hook, struct sk_buff *pskb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*));

/****************************************
 * func: 处理PreRouting阶段的钩子函数
****************************************/
unsigned int NET_HookPreRouting(unsigned int hook, struct sk_buff *pskb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*));

/****************************************
 * func: 处理PostRouting阶段的钩子函数
****************************************/
unsigned int NET_HookPostRouting(unsigned int hook, struct sk_buff *pskb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*));

/****************************************
 * func: 处理Forward阶段的钩子函数
****************************************/
unsigned int NET_HookForward(unsigned int hook, struct sk_buff *pskb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*));

/****************************************
 * func: 开启数据包的截取
 * return: true = 成功   false = 失败
****************************************/
bool startHook();

/****************************************
 * func: 关闭数据包的截取
 * return: void
****************************************/
void stopHook();

/***************************************
 * func: 添加一个PortPackage数据
 * return: void
 * @para temp: 要添加的PortPackage数据
***************************************/
void AddPortPackage(Port_Map_Package temp);

/***************************************
 * func: 查找一个端口Port_Map_Package数据
 * return: 返回当前端口在PortPackageData中的位置   NULL = 没有找到对用的_port
 * @para _port: 要查找的端口号
****************************************/
Port_Map_Package *FindPortPackageByPortProtocol(unsigned int _port, char _protocolType);

/***************************************
 * func: 清空PortPackageData链表数据
 * return: void
***************************************/
void ClearPortPackage();

/****************************************
 * func: 从/proc/net/tcp文件或者udp文件中获取一行数据中的十六进制端口号和对应的inode
 * return: -1 = 没有端口信息   >0 = 获取了这行数据中端口对应的inode号
 * @para str: 一行的数据
 * @para hexPort: 存放十六进制的端口号
****************************************/
int getPortFromStr(char *str, char *hexPort);

/****************************************
 * func: 从/proc/net/tcp或者udp文件中获取要查找的端口对应的inode
 * return: -1 = 没有找到    >0 = 对应的inode
 * @para path: 要查找的文件(/proc/net/tcp)
 * @para hex: 要查找的十六进制端口
****************************************/
int getInodeByHexPort(char *path, char *hex);

/****************************************
 * func: 通过端口号查找对应的inode
 * return: -1 = 没有找到   >0 = inode
 * @para port: 十进制的端口号
 * @para protocol: 网络协议  'T' = tcp协议   'U' = udp协议
*****************************************/
int getInodeByPort(int port, char protocol);

/****************************************
 * func: 判断/proc/pid/fd/下的符号链接是否为socket
 * return: -1 = 不是或者错误   >0 = 返回该符号链接所指向的inode
 * @para info: 符号链接里的数据
****************************************/
int judgeSocketLink(char *info);

/******************************************
 * func: 映射端口和进程的关系
 * return: false = 该端口不属于这个进程    true = 该端口属于该进程
 * @para ProcPath: 要判断的进程/proc目录  例如/proc/1234
 * @para portInfo: 截取到的端口信息
******************************************/
bool mapProcessPort(char *ProcPath, Port_Map_Package portInfo);

#endif
