/******************************************************
* Author       : fengzhimin
* Create       : 2018-03-01 04:41
* Last modified: 2018-05-17 22:13
* Email        : 374648064@qq.com
* Filename     : conflictResolution.c
* Description  : the resolution of resource conflict
******************************************************/

#include "resolution/conflictResolution.h"

static char error_info[200];
static char lineData[LINE_CHAR_MAX_NUM];

bool ExecuteCommandDebug(char *commandArgv[], const char *file, const char *function, const int line)
{
	pid_t pid = fork();
	if(pid < 0 )
	{
		WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
		sprintf(error_info, "create process failed: %s.\n", strerror(errno));
		Error(error_info);

		return false;
	}
	else if(pid == 0)
	{
		if(execvp("sh", commandArgv) < 0)
		{
			WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
			sprintf(error_info, "execute command(%s) failure: %s.\n", commandArgv[1], strerror(errno));
			Error(error_info);

			exit(-1);
		}
	}
	else
	{
		int status;
		if(wait(&status) < 0)
		{
			WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
			sprintf(error_info, "wait child process(%s) exit failed: %s.\n", commandArgv[1], strerror(errno));
			Error(error_info);
			
			return false;
		}
		else if(status == 0)
			return true;
		else
			return false;
	}

	return false;
}

bool ReduceConfDebug(char *softwareName, char *confName, const char *file, const char *function, const int line)
{
	char scriptPath[SCRIPT_PATH_MAX_NUM];
	sprintf(scriptPath, "%s/%sDown.sh", ResourceMonitor_Client_SOLUTION_PATH, softwareName);
	if((access(scriptPath, F_OK)) == -1)
	{
		WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
		sprintf(error_info, "script(%s) is not existing\n", scriptPath);
		Error(error_info);

		return false;
	}

	char *commandArgv[] = { "sh", scriptPath, confName, NULL};
	if(ExecuteCommand(commandArgv))
		return true;
	else
	{
		WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
		sprintf(error_info, "reduce config failure(software:%s\tconfName:%s)\n", softwareName, confName);
		Error(error_info);

		return false;
	}

}

bool IncreaseConfDebug(char *softwareName, char *confName, char *defValue, const char *file, const char *function, const int line)
{
	char scriptPath[SCRIPT_PATH_MAX_NUM];
	sprintf(scriptPath, "%s/%sUp.sh", ResourceMonitor_Client_SOLUTION_PATH, softwareName);
	if((access(scriptPath, F_OK)) == -1)
	{
		WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
		sprintf(error_info, "script(%s) is not existing\n", scriptPath);
		Error(error_info);

		return false;
	}

	char *commandArgv[] = { "sh", scriptPath, confName, defValue, NULL};
	if(ExecuteCommand(commandArgv))
		return true;
	else
	{
		WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
		sprintf(error_info, "increase config failure(software:%s\tconfName:%s)\n", softwareName, confName);
		Error(error_info);

		return false;
	}

}
