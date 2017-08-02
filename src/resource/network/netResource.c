/******************************************************
* Author       : fengzhimin
* Create       : 2017-04-27 06:40
* Last modified: 2017-05-02 19:01
* Email        : 374648064@qq.com
* Filename     : netResource.c
* Description  : 
******************************************************/

#include "resource/network/netResource.h"

static char error_info[200];
static char lineData[LINE_CHAR_MAX_NUM];
static char subStr[17][MAX_SUBSTR];


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
			{
				currentPortPackageData->inPackageSize++;
				currentPortPackageData->inDataBytes += skb->len;
			}
			else
			{
				currentPortPackageData->outPackageSize++;
				currentPortPackageData->outDataBytes += skb->len;
			}
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
				temp.inDataBytes = skb->len;
				temp.outDataBytes = 0;
			}
			else
			{
				temp.inPackageSize = 0;
				temp.outPackageSize = 1;
				temp.inDataBytes = 0;
				temp.outDataBytes = skb->len;
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
			{
				currentPortPackageData->inPackageSize++;
				currentPortPackageData->inDataBytes += skb->len;
			}
			else
			{
				currentPortPackageData->outPackageSize++;
				currentPortPackageData->outDataBytes += skb->len;
			}
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
				temp.inDataBytes = skb->len;
				temp.outDataBytes = 0;
			}
			else
			{
				temp.inPackageSize = 0;
				temp.outPackageSize = 1;
				temp.inDataBytes = 0;
				temp.outDataBytes = skb->len;
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

bool startHookDebug(const char *file, const char *function, const int line)
{
	//每次开始截取数据包的时候清空先前所获取的数据
	ClearPortPackage();
	//这里只能用kmalloc的GFP_ATOMIC模式，vmalloc可能会造成休眠
	beginPortPackageData = currentPortPackageData = endPortPackageData = PortPackageData = kmalloc(sizeof(Port_Map_Package), GFP_ATOMIC);
	PortPackageData->port = -1;  //申请列表头
	PortPackageData->inPackageSize = PortPackageData->outPackageSize = 0;
	PortPackageData->inDataBytes = PortPackageData->outDataBytes = 0;
	PortPackageData->protocol = '0';
	PortPackageData->next = NULL;
	int ret = 0;
	//安装钩子函数
	ret = nf_register_hooks(net_hooks, ARRAY_SIZE(net_hooks));
	if(ret)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
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
	(*endPortPackageData) = temp;
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

int getPortFromStr(char *str, char *hexPort)
{
	int strLength = strlen(str);
	int i, point = 0, index = 0;
	int ret = 0;
	for(i = 0; i < strLength; i++)
	{
		//具体实现方法参考/proc/net/tcp文件格式
		if(point == 2)
		{
			if(str[i] == ' ')
			{
				int m_point = 0;
				bool continue_space = false;
				for( i++; i < strLength; i++)
				{
					if(m_point == 7)
					{
						if(str[i] == ' ')
							return ret;
						ret = ret*10 + str[i] - '0';    //inode
					}
					else if((str[i] == ' ' || str[i] == '\t') && !continue_space)
					{
						continue_space = true;
						m_point++;
					}
					else if(str[i] != ' ' && str[i] != '\t')
						continue_space = false;
				}
			}
			hexPort[index++] = str[i];
		}
		else if(str[i] == ':')
		{
			point++;
		}
	}

	return -1;
}

int getInodeByHexPortDebug(char *path, char *hex, const char *file, const char *function, const int line)
{
	struct file *fp = KOpenFile(path, O_RDONLY);
	if(fp == NULL)
	{
		/*
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", path, " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		*/
		return -1;
	}
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	KReadLine(fp, lineData);   //读取文件的第一行   第一行中不包含端口信息
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	char hexPort[HEX_MAX_NUM];
	int ret = -1;
	while(KReadLine(fp, lineData) == -1)
	{
		memset(hexPort, 0, HEX_MAX_NUM);
		ret = getPortFromStr(lineData, hexPort);
		if(strcasecmp(hexPort, hex) == 0)
		{
			KCloseFile(fp);
			return ret;
		}
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
	}

	KCloseFile(fp);

	return -1;
}

int getInodeByPort(int port, char protocol)
{
	char hexPort[HEX_MAX_NUM];
	memset(hexPort, 0, HEX_MAX_NUM);
	Dec2Hex(hexPort, port);
	int ret = -1;
	switch(protocol)
	{
	case 'T':
		ret = getInodeByHexPort("/proc/net/tcp", hexPort);
		if(ret != -1)
			return ret;
		else if((ret = getInodeByHexPort("/proc/net/tcp6", hexPort)) != -1)
			return ret;
		break;
	case 'U':
		ret = getInodeByHexPort("/proc/net/udp", hexPort);
		if(ret != -1)
			return ret;
		else if((ret = getInodeByHexPort("/proc/net/udp6", hexPort)) != -1)
			return ret;
		break;
	}

	return ret;
}

/*
int judgeSocketLink(char *info)
{
	int infoLength = strlen(info);
	int i;
	for( i = 0; i < infoLength; i++)
	{

	}
}*/

bool IsSocketLink(char *ProcPath, int *port)
{
	memset(port, 0, MAX_PORT_NUM*sizeof(int));
	int index = 0;
	char fdPath[MAX_PROCPATH];
	memset(fdPath, 0, MAX_PROCPATH);
	sprintf(fdPath, "%s/fd", ProcPath);
	int fdDir = vfs_opendir(fdPath);
	if(fdDir == -1)
	{
		return false;
	}
	
	struct KCode_dirent *begin;
	struct KCode_dirent *cur;
	begin = cur = vfs_readdir(fdDir);
	vfs_closedir(fdDir);
	char buflinkInfo[LINK_MAX_NUM];
	char path[MAX_PROCPATH];
	bool ret = false;
	while(cur != NULL)
	{
		//只处理符号链接
		if(cur->type == DT_LNK)
		{
			memset(buflinkInfo, 0, LINK_MAX_NUM);
			memset(path, 0, MAX_PROCPATH);
			sprintf(path, "%s/%s", fdPath, cur->name);
			int retLink = vfs_readlink(path, buflinkInfo, LINK_MAX_NUM);
			if(retLink > 0 && retLink <= LINK_MAX_NUM)
			{
				buflinkInfo[retLink] = '\0';
				if(retLink > 7)
				{
					//judge whether buflinkInfo contain socket string or not
					if(buflinkInfo[0] == 's' && buflinkInfo[1] == 'o' && buflinkInfo[2] == 'c' && buflinkInfo[3] == 'k')
					{
						if(index >= MAX_PORT_NUM)
							break;
						port[index++]  = ExtractNumFromStr(buflinkInfo);
						ret = true;
					}
				}
			}
		}

		cur = cur->next;
	}
	//释放读取文件夹资源
	vfs_free_readdir(begin);
	return ret;
}

int getAllNetStateDebug(NetInfo **beginNetInfo, const char *file, const char *function, const int line)
{
	int netCardNum = 0;
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	int lineNum = 1;
	struct file *fp = KOpenFile("/proc/net/dev", O_RDONLY);
	if(fp == NULL)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", "/proc/net/dev", " 失败！ 错误信息： ", "   ", "\n");
		RecordLog(error_info);
		return 0;
	}
	NetInfo *tailNetInfo = NULL;
	(*beginNetInfo) = NULL;
	while(KReadLine(fp, lineData) == -1)
	{
		if(lineNum >= 3)
		{
			if(tailNetInfo == NULL)
			{
				(*beginNetInfo) = tailNetInfo = vmalloc(sizeof(NetInfo));
			}
			else
			{
				tailNetInfo = tailNetInfo->next = vmalloc(sizeof(NetInfo));
			}
			memset(tailNetInfo, 0, sizeof(NetInfo));
			cutStrByLabel(lineData, ' ', subStr, 17);
			subStr[0][strlen(subStr[0])-1] = '\0';
			strcpy(tailNetInfo->netCardName, subStr[0]);
			tailNetInfo->netCardInfo.downloadBytes = ExtractNumFromStr(subStr[1]);
			tailNetInfo->netCardInfo.uploadBytes = ExtractNumFromStr(subStr[9]);
			tailNetInfo->netCardInfo.downloadPackage = ExtractNumFromStr(subStr[2]);
			tailNetInfo->netCardInfo.uploadPackage = ExtractNumFromStr(subStr[10]);
			tailNetInfo->next = NULL;
			netCardNum++;
		}
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
		lineNum++;
	}
	if(lineNum == 1)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", "/proc/net/dev", " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		KCloseFile(fp);
		return 0;
	}
	KCloseFile(fp);
	return netCardNum;
}

int getAllNetCardNameDebug(char **netCardName, unsigned int size, const char *file, const char *function, const int line)
{
	memset(lineData, 0, LINE_CHAR_MAX_NUM);
	memset((*netCardName), 0, size);
	int lineNum = 1;
	int netCardNum = 0;
	struct file *fp = KOpenFile("/proc/net/dev", O_RDONLY);
	if(fp == NULL)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "打开文件: ", "/proc/net/dev", " 失败！ 错误信息： ", "   ", "\n");
		RecordLog(error_info);
		return 0;
	}
	char _netCardName[MAX_NETCARDNAME_LENGTH+1];
	int i;
	while(KReadLine(fp, lineData) == -1)
	{
		if(lineNum >= 3)
		{
			memset(_netCardName, 0, MAX_NETCARDNAME_LENGTH+1);
			for(i = 0; i < MAX_NETCARDNAME_LENGTH; i++)
			{
				if(lineData[i] == ':')
					break;
				_netCardName[i] = lineData[i];
			}

			//将多个网卡通过:合并在一起
			if((strlen((*netCardName))+strlen(_netCardName)+1) >= size)
			{
				WriteLog("logInfo.log", "调用者信息\n", file, function, line);
				RecordLog("预存所有网卡名的数组小于所有网卡名的长度!\n");
				KCloseFile(fp);
				return netCardNum;
			}
			if(i == MAX_NETCARDNAME_LENGTH)
			{
				WriteLog("logInfo.log", "调用者信息\n", file, function, line);
				RecordLog("网卡名的字符长度大于预设值!\n");
			}
			else
			{
				sprintf((*netCardName), "%s:%s", (*netCardName), _netCardName);
				netCardNum++;
			}
		}
		memset(lineData, 0, LINE_CHAR_MAX_NUM);
		lineNum++;
	}
	if(lineNum == 1)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		sprintf(error_info, "%s%s%s%s%s", "读取文件: ", "/proc/net/dev", " 失败！ 错误信息： ", "    ", "\n");
		RecordLog(error_info);
		KCloseFile(fp);
		return 0;
	}
	KCloseFile(fp);
	return netCardNum;
}

int getNetCardSpeedDebug(char *netCardName, const char *file, const char *function, const int line)
{
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, netCardName);
	int sockfd = vfs_socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		RecordLog("建立socket失败!\n");
		return 0;
	}
	int err;
	struct ethtool_cmd ecmd;
	ecmd.cmd = ETHTOOL_GSET;
	ifr.ifr_data = (caddr_t)&ecmd;
	err = vfs_ioctl(sockfd, SIOCETHTOOL, &ifr);
	vfs_socketClose(sockfd);
	if(err == 0)
	{
		return ecmd.speed;
	}
	else
	{
		WriteLog("logInfo.log", "调用者信息\n", file, function, line);
		RecordLog("vfs_ioctl执行失败!\n");
		return 0;
	}
}
