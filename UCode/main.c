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
#include "config.h"

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
	if(pthread_create(&thr, NULL, monitorPort, NULL) != 0)
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
				break;
			printf("conflict type = %d\t conflict process = %s\n", info.conflictInfo.conflictType, info.conflictInfo.processInfo.name);
		}
	    usleep(100000);
    }
    close(skfd);

    free((void *)nlh);
    return 0;

}

