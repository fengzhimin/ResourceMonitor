/******************************************************
* Author       : fengzhimin
* Create       : 2017-05-07 15:33
* Last modified: 2017-05-07 15:33
* Email        : 374648064@qq.com
* Filename     : kernel2user.c
* Description  : 
******************************************************/

#include "messagePassing/kernel2user.h"

static char error_info[200];
static unsigned int pid;   //标记不同的用户层进程
static struct sock *netlinkfd = NULL;    //netlink句柄
static struct netlink_kernl_cfg cfg = 
{
	.input = recv_Msg,    //接收的回调函数
};

struct sock *init_Netlink()
{
	netlinkfd = netlink_kernel_create(&init_net, NETLINK_USER_MSG, &cfg);
	return netlinkfd;
}

void release_Netlink()
{
	sock_release(netlinkfd->sk_socket);
}

int send_Msg(int8_t *pbuf, unsigned int len)
{
	struct sk_buff *nl_skb;
	struct nlmsghdr *nlh;
	int ret = -1;
	nl_skb = nlmsg_new(len, GFP_ATOMIC);
	if(!nl_skb)
	{
		RecordLog("netlink_alloc_skb error!\n");
		return ret;
	}
	nlh = nlmsg_put(nl_skb, 0, 0 NETLINK_USER_MSG, len, 0);
	if(nlh == NULL)
	{
		RecordLog("nlmsg_put() error!\n");
		nlmsg_free(nl_skb);
		return ret;
	}
	memcpy(nlmsg_data(nlh), pbuf, len);
	//向编号为pid的用户层进程发送数据
	ret = netlink_unicast(netlinkfd, nl_skb, pid, MSG_DONTWAIT);
	return ret;
}

void recv_Msg(struct sk_buff *skb)
{
	struct nlmsghdr *nlh = NULL;
	void *data = nULL;
	if(skb->len >= nlmsg_total_size(0))
	{
		nlh = nlmsg_hdr(skb);
		data = NLMSG_DATA(nlh);  //获取数据包中的数据
		pid = nlh->nlmsg_pid;    //设置用户层的id
		if(data)
			send_Msg(data, nlmsg_len(nlh));
	}
}
