/*******************************
file:           u_netlink.c
description:    netlink demo
author:         arvik
email:          1216601195@qq.com
blog:           http://blog.csdn.net/u012819339
*******************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <stddef.h>
#include "config.h"
#include "common/confOper.h"

#define NETLINK_USER 22
#define USER_MSG    (NETLINK_USER + 1)
#define MSG_LEN 100
#define DATA_SPACE   100

struct _my_msg
{
    struct nlmsghdr hdr;
	ConflictProcInfo conflictInfo;
};

void monitorPort()
{
	struct stat statFile;
	char *filePath = "/etc/conflictPortInfo.info";
	long long lastmodification = 0;
	while(1)
	{
		stat(filePath, &statFile);
		if(lastmodification != statFile.st_mtime)
		{
			lastmodification = statFile.st_mtime;
			int fd = open(filePath, O_RDONLY);
			if(fd >= 0)
			{
				char buf[1024] = {'\0'};
				read(fd, buf, 1024);
				printf("%s\n", buf);
				close(fd);
			}
		}
		usleep(100000);
	}
}

int main(int argc, char **argv)
{
	pthread_t thr;
	if(pthread_create(&thr, NULL, (void *)monitorPort, NULL) != 0)
	{
		printf("create thread failure!\n");
	}
    char *data = "request";
    struct sockaddr_nl  local, dest_addr;

    int skfd;
    struct nlmsghdr *nlh = NULL;
    struct _my_msg info;
    int ret;

    skfd = socket(AF_NETLINK, SOCK_RAW, USER_MSG);
    if(skfd == -1)
    {
        printf("create socket error...%s\n", strerror(errno));
        return -1;
    }

    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = getpid(); 
    local.nl_groups = 0;
    if(bind(skfd, (struct sockaddr *)&local, sizeof(local)) != 0)
    {
        printf("bind() error\n");
        close(skfd);
        return -1;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; // to kernel
    dest_addr.nl_groups = 0;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(DATA_SPACE));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(DATA_SPACE);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = local.nl_pid; //self port
	bool symbol = false;
	char label[CONFIG_LABEL_MAX_NUM];
	char value[CONFIG_VALUE_MAX_NUM];
	time_t timer;
	struct tm *tblock;
    memcpy(NLMSG_DATA(nlh), data, strlen(data));

    while(1)
    {
	    ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_nl));

	    if(!ret)
	    {
			perror("sendto error1\n");
			close(skfd);
			exit(-1);
	    }
		symbol = true;
		while(1)
		{
			memset(&info, 0, sizeof(struct _my_msg));
			ret = recvfrom(skfd, &info, sizeof(struct _my_msg), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
			if(!ret)
			{
				perror("recv form kernel error\n");
				close(skfd);
				exit(-1);
			}
			if(info.conflictInfo.conflictType == 0)
			{
				break;
			}
			if(symbol)
			{
				//clear history info
				system("clear");
				timer = time(NULL);
				tblock = localtime(&timer);
				printf("Local time is: %s\n", asctime(tblock));
				symbol = false;
			}
			printf("\033[31mconflict type = %2d conflict process = %20s", info.conflictInfo.conflictType, info.conflictInfo.name);
			printf("[");
			if(info.conflictInfo.conflictType & CPU_CONFLICT)
				printf("CPU ");
			if(info.conflictInfo.conflictType & MEM_CONFLICT)
				printf("MEM ");
			if(info.conflictInfo.conflictType & IO_CONFLICT)
				printf("IO ");
			if(info.conflictInfo.conflictType & NET_CONFLICT)
				printf("NET ");
			if(info.conflictInfo.conflictType & PORT_CONFLICT)
				printf("PORT ");
			printf("]\033[0m\n");
			//CPU
			if(info.conflictInfo.conflictType & CPU_CONFLICT)
			{
				memset(label, 0, CONFIG_LABEL_MAX_NUM);
				memset(value, 0, CONFIG_VALUE_MAX_NUM);
				strcpy(label, info.conflictInfo.name);
				strcat(label, "/CPU");
				if(getConfValueByLabelAndKey(label, "name", value))
				{
					printf("\033[32mCPU: %s\033[0m\n", value);
				}
			}
			//MEM
			if(info.conflictInfo.conflictType & MEM_CONFLICT)
			{
				memset(label, 0, CONFIG_LABEL_MAX_NUM);
				memset(value, 0, CONFIG_VALUE_MAX_NUM);
				strcpy(label, info.conflictInfo.name);
				strcat(label, "/MEM");
				if(getConfValueByLabelAndKey(label, "name", value))
				{
					printf("\033[32mMEM: %s\033[0m\n", value);
				}
			}
			//IO
			if(info.conflictInfo.conflictType & IO_CONFLICT)
			{
				memset(label, 0, CONFIG_LABEL_MAX_NUM);
				memset(value, 0, CONFIG_VALUE_MAX_NUM);
				strcpy(label, info.conflictInfo.name);
				strcat(label, "/IO");
				if(getConfValueByLabelAndKey(label, "name", value))
				{
					printf("\033[32mIO: %s\033[0m\n", value);
				}
			}
			//NET
			if(info.conflictInfo.conflictType & NET_CONFLICT)
			{
				memset(label, 0, CONFIG_LABEL_MAX_NUM);
				memset(value, 0, CONFIG_VALUE_MAX_NUM);
				strcpy(label, info.conflictInfo.name);
				strcat(label, "/NET");
				if(getConfValueByLabelAndKey(label, "name", value))
				{
					printf("\033[32mNET: %s\033[0m\n", value);
				}
			}
			//PORT
			if(info.conflictInfo.conflictType & PORT_CONFLICT)
			{
				memset(label, 0, CONFIG_LABEL_MAX_NUM);
				memset(value, 0, CONFIG_VALUE_MAX_NUM);
				strcpy(label, info.conflictInfo.name);
				strcat(label, "/PORT");
				if(getConfValueByLabelAndKey(label, "name", value))
				{
					printf("\033[32mPORT: %s\033[0m\n", value);
				}
			}

			printf("冲突前资源使用: CPU-%d\tMEM-%d\tSWAP-%d\tIO-%lld\tNET-%lld\n", info.conflictInfo.normalResUsed.cpuUsed, info.conflictInfo.normalResUsed.memUsed, info.conflictInfo.normalResUsed.swapUsed, info.conflictInfo.normalResUsed.ioDataBytes, info.conflictInfo.normalResUsed.netTotalBytes);

			printf("冲突时资源使用: CPU-%d\tMEM-%d\tSWAP-%d\tIO-%lld\tNET-%lld\n", info.conflictInfo.conflictResUsed.cpuUsed, info.conflictInfo.conflictResUsed.memUsed, info.conflictInfo.conflictResUsed.swapUsed, info.conflictInfo.conflictResUsed.ioDataBytes, info.conflictInfo.conflictResUsed.netTotalBytes);
		}
	    usleep(100000);
    }
    close(skfd);

    free((void *)nlh);
    return 0;

}

