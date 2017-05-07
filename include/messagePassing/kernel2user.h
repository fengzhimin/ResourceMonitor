/******************************************************
* Author       : fengzhimin
* Create       : 2017-05-07 15:06
* Last modified: 2017-05-07 15:06
* Email        : 374648064@qq.com
* Filename     : kernel2user.h
* Description  : 
******************************************************/

#ifndef __KERNEL2USER_H__
#define __KERNEL2USER_H__

#include <linux/netlink.h>
#include <net/sock.h>
#include "config.h"
#include "log/logOper.h"


/***********************************
 * func: 初始化netlink
 * return: NULL = 失败　　>0 = 成功
***********************************/
struct sock *init_Netlink();

/**********************************
 * func: 释放netlink
 * return: void
**********************************/
void release_Netlink();

/****************************************
 * func: 发送数据
 * return: <= 0 == 失败　　> 0 == 程序
 * @para pbuf: 要发送的数据
 * @para len: 数据的长度
****************************************/
int send_Msg(int8_t *pbuf, unsigned int len);

/****************************************
 * func: 接收数据
 * return: void
 * @para skb: 接收到的数据包
****************************************/
void recv_Msg(struct sk_buff *skb);

#endif
