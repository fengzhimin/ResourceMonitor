/******************************************************
* Author       : fengzhimin
* Create       : 2018-11-23 06:53
* Last modified: 2018-11-23 19:24
* Email        : 374648064@qq.com
* Filename     : procInfo.c
* Description  : 设置进程信息相关函数
******************************************************/

#include "common/procInfo.h"

static char **g_main_Argv = NULL;    /* pointer to argument vector */
static char *g_main_LastArgv = NULL;    /* end of argv */
static char error_info[200];

void setproctitle_init(int argc, char **argv, char **envp)
{
    int i;

    for (i = 0; envp[i] != NULL; i++) // calc envp num
        continue;
    environ = (char **) malloc(sizeof (char *) * (i + 1)); // malloc envp pointer

    for (i = 0; envp[i] != NULL; i++)
    {
        environ[i] = malloc(sizeof(char) * strlen(envp[i]));
        strcpy(environ[i], envp[i]);
    }
    environ[i] = NULL;

    g_main_Argv = argv;
    if (i > 0)
        g_main_LastArgv = envp[i - 1] + strlen(envp[i - 1]);
    else
        g_main_LastArgv = argv[argc - 1] + strlen(argv[argc - 1]);
}

void setproctitle(const char *fmt, ...)
{
    char *p;
    int i;
    char buf[MAX_ARGV_LENGTH];

    va_list ap;
    p = buf;

    va_start(ap, fmt);
    vsprintf(p, fmt, ap);
    va_end(ap);

    i = strlen(buf);

    if (i > g_main_LastArgv - g_main_Argv[0] - 2)
    {
        i = g_main_LastArgv - g_main_Argv[0] - 2;
        buf[i] = '\0';
    }
    //修改argv[0]
    (void) strcpy(g_main_Argv[0], buf);

    p = &g_main_Argv[0][i];
    while (p < g_main_LastArgv)
        *p++ = '\0';
    g_main_Argv[1] = NULL;
    
    //调用prctl
    prctl(PR_SET_NAME,buf);
}

int getProcIdByName(char *procName)
{
	char command[512] = {0};
	int ret = 0;
	sprintf(command, "ps -e | grep \'%s\' | awk \'{print $1}\'", procName);
	FILE *fp = popen(command, "r");
	if(fp == NULL)
	{
		sprintf(error_info, "execute \'%s\' command failure!\n", command);
		Error(error_info);
		return -1;
	}
	char pid[10] = {0};
	int tmp_pid;
	while(NULL != fgets(pid, 10, fp))
	{
		//排除当前进程
		tmp_pid = StrToInt(pid);
		if(tmp_pid != getpid())
		{
			ret = tmp_pid;
			break;
		}
	}

	pclose(fp);
	return ret;
}
