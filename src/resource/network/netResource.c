/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-27 06:40
* Last modified: 2017-04-27 06:40
* Email        : 374648064@qq.com
* Filename     : netResource.c
* Description  : 
******************************************************/

#include "resource/network/netResource.h"

//static char error_info[200];


//定义钩子函数
static struct nf_hook_ops net_hooks[] = {
	{
		.hook     = NET_HookLocalIn,
		.pf       = PF_INET,
		.hooknum  = NF_INET_LOCAL_IN,
		.priority = NF_IP_PRI_FILTER-1,
	},
	{
		.hook     = NET_HookLocalOut,
		.pf       = PF_INET,
		.hooknum  = NF_INET_LOCAL_OUT,
		.priority = NF_IP_PRI_FILTER-1,
	},
	{
		.hook     = NET_HookForward,
		.pf       = PF_INET,
		.hooknum  = NF_INET_FORWARD,
		.priority = NF_IP_PRI_FILTER-1,
	},
	{
		.hook     = NET_HookPreRouting,
		.pf       = PF_INET,
		.hooknum  = NF_INET_PRE_ROUTING,
		.priority = NF_IP_PRI_FILTER-1,
	},
	{
		.hook     = NET_HookPostRouting,
		.pf       = PF_INET,
		.hooknum  = NF_INET_POST_ROUTING,
		.priority = NF_IP_PRI_FILTER-1,
	},
};

unsigned int filter_http(char *type, struct sk_buff *pskb)
{
	int _port;
	struct sk_buff *skb = pskb;
	struct iphdr *iph = ip_hdr(skb);   //获取IP头
	//判断数据包类型
	//过滤出tcp和udp协议的数据包
	if(iph->protocol == IPPROTO_TCP)
	{
		struct tcphdr *tcp = tcp_hdr(skb);
		/*
		printk("%s: %d.%d.%d.%d ==> %d.%d.%d.%d (%u --> %u)\n", type, \
				(iph->saddr & 0x000000FF)>>0, (iph->saddr & 0x0000FF00) >> 8, \
				(iph->saddr & 0x00FF0000)>>16, (iph->saddr & 0xFF000000) >> 24, \
				(iph->daddr & 0x000000FF)>>0, (iph->daddr & 0x0000FF00) >> 8, \
				(iph->daddr & 0x00FF0000)>>16, (iph->daddr & 0xFF000000) >> 24, \
				htons(tcp->source), htons(tcp->dest)
				);
		*/
		//获取要提取的本地进程的端口号
		if(strcmp(type, "in") == 0)
			_port = htons(tcp->dest);
		else
			_port = htons(tcp->source);
		currentPortPackageData = FindPortPackageByPortProtocol(_port, 'T');
		//判断是否已经记录过该端口数据
		if(currentPortPackageData != NULL)
		{
			//判断是输入数据包还是输出数据包
			if(strcmp(type, "in") == 0)
				currentPortPackageData->inPackageSize++;
			else
				currentPortPackageData->outPackageSize++;
		}
		else
		{
			//新增数据包
			Port_Map_Package temp;
			temp.port = _port;
			temp.protocol = 'T';
			if(strcmp(type, "in") == 0)
			{
				temp.inPackageSize = 1;
				temp.outPackageSize = 0;
			}
			else
			{
				temp.inPackageSize = 0;
				temp.outPackageSize = 1;
			}
			temp.next = NULL;
			AddPortPackage(temp);
		}
	}
	else if(iph->protocol == IPPROTO_UDP)
	{
		struct udphdr *udp = udp_hdr(skb);
		/*
		printk("%s: %d.%d.%d.%d ==> %d.%d.%d.%d (%u --> %u)\n", type, \
				(iph->saddr & 0x000000FF)>>0, (iph->saddr & 0x0000FF00) >> 8, \
				(iph->saddr & 0x00FF0000)>>16, (iph->saddr & 0xFF000000) >> 24, \
				(iph->daddr & 0x000000FF)>>0, (iph->daddr & 0x0000FF00) >> 8, \
				(iph->daddr & 0x00FF0000)>>16, (iph->daddr & 0xFF000000) >> 24, \
				htons(udp->source), htons(udp->dest)
				);
		*/
		if(strcmp(type, "in") == 0)
			_port = htons(udp->dest);
		else
			_port = htons(udp->source);
		currentPortPackageData = FindPortPackageByPortProtocol(_port, 'U');
		if(currentPortPackageData != NULL)
		{
			if(strcmp(type, "in") == 0)
				currentPortPackageData->inPackageSize++;
			else
				currentPortPackageData->outPackageSize++;
		}
		else
		{
			Port_Map_Package temp;
			temp.port = _port;
			temp.protocol = 'U';
			if(strcmp(type, "in") == 0)
			{
				temp.inPackageSize = 1;
				temp.outPackageSize = 0;
			}
			else
			{
				temp.inPackageSize = 0;
				temp.outPackageSize = 1;
			}
			temp.next = NULL;
			AddPortPackage(temp);
		}
	}

	return NF_ACCEPT;
}

unsigned int NET_HookLocalIn(unsigned int hook, struct sk_buff *pskb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*))
{
	return filter_http("in", pskb);
}

unsigned int NET_HookLocalOut(unsigned int hook, struct sk_buff *pskb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*))
{
	return filter_http("out", pskb);
}

unsigned int NET_HookPreRouting(unsigned int hook, struct sk_buff *pskb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*))
{
	return NF_ACCEPT;
}

unsigned int NET_HookPostRouting(unsigned int hook, struct sk_buff *pskb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*))
{
	return NF_ACCEPT;
}

unsigned int NET_HookForward(unsigned int hook, struct sk_buff *pskb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff*))
{
	return NF_ACCEPT;
}

bool startHook()
{
	//每次开始截取数据包的时候清空先前所获取的数据
	ClearPortPackage();
	//这里只能用kmalloc的GFP_ATOMIC模式，vmalloc可能会造成休眠
	beginPortPackageData = currentPortPackageData = endPortPackageData = PortPackageData = kmalloc(sizeof(Port_Map_Package), GFP_ATOMIC);
	PortPackageData->port = -1;  //申请列表头
	PortPackageData->inPackageSize = PortPackageData->outPackageSize = 0;
	PortPackageData->protocol = '0';
	PortPackageData->next = NULL;
	int ret = 0;
	//安装钩子函数
	ret = nf_register_hooks(net_hooks, ARRAY_SIZE(net_hooks));
	if(ret)
	{
		RecordLog("register hook failed\n");
		return false;
	}
	else
		return true;
}

void stopHook()
{
	//卸载钩子函数
	nf_unregister_hooks(net_hooks, ARRAY_SIZE(net_hooks));
}

Port_Map_Package *FindPortPackageByPortProtocol(unsigned int _port, char _protocolType)
{
	currentPortPackageData = beginPortPackageData;
	while(currentPortPackageData != NULL)
	{
		if(currentPortPackageData->port == _port && currentPortPackageData->protocol == _protocolType)
			return currentPortPackageData;
		else
			currentPortPackageData = currentPortPackageData->next;
	}

	return NULL;
}

void AddPortPackage(Port_Map_Package temp)
{
	//在列表的末尾新增一个端口数据
	endPortPackageData = endPortPackageData->next = kmalloc(sizeof(Port_Map_Package), GFP_ATOMIC);
	endPortPackageData->port = temp.port;
	endPortPackageData->protocol = temp.protocol;
	endPortPackageData->inPackageSize = temp.inPackageSize;
	endPortPackageData->outPackageSize = temp.outPackageSize;
	endPortPackageData->next = NULL;
}

void ClearPortPackage()
{
	//定位到列表的首元素，从首元素开始删除数据
	currentPortPackageData = beginPortPackageData;
	while(currentPortPackageData != NULL)
	{
		beginPortPackageData = beginPortPackageData->next;
		kfree(currentPortPackageData);
		currentPortPackageData = beginPortPackageData;
	}
}
