/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-27 06:38
* Last modified: 2017-04-27 06:38
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

#endif
