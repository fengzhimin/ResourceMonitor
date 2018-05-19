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

bool ExecuteCommandDebug(char *command, const char *file, const char *function, const int line)
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
		if(execl("/bin/sh", "sh", "-c", command, NULL) < 0)
		{
			WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
			sprintf(error_info, "execute command(%s) failure: %s.\n", command, strerror(errno));
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
			sprintf(error_info, "wait child process exit failed: %s.\n", strerror(errno));
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

bool getConfigValueDebug(char *getCommand, char *value, const char *file, const char *function, const int line)
{
	strcat(getCommand, " > ");
	strcat(getCommand, GET_CONFIG_VALUE_FILE);
	bool ret = ExecuteCommand(getCommand);
	if(ret)
	{
		int fd = OpenFile(GET_CONFIG_VALUE_FILE, O_RDONLY);
		if(fd == -1)
		{
			WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
			sprintf(error_info, "open %s file failure: %s.\n", GET_CONFIG_VALUE_FILE, strerror(errno));
			Error(error_info);

			return false;
		}

		memset(lineData, 0, LINE_CHAR_MAX_NUM);

		if(ReadLine(fd, lineData))
		{
			strcpy(value, lineData);
		}
		
		CloseFile(fd);
	}

	return ret;
}

bool ResolveContentionDebug(char *softwareName, char *configName, const char *file, const char *function, const int line)
{
	char value[CONFIG_VALUE_MAX_NUM];
	char configValue[CONFIG_VALUE_MAX_NUM];
	char setCommand[CONFIG_VALUE_MAX_NUM];
	char getCommand[CONFIG_VALUE_MAX_NUM];

	//获取获得配置项当前值的命令
	if(getConfValueByLabelAndKey(softwareName, "getCommand", value))
	{
		sprintf(getCommand, value, configName);
		//获取配置项值
		getConfigValue(getCommand, configValue);
	}
	else
	{
		WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
		sprintf(error_info, "label:%s key:getCommand failure.\n", softwareName);
		Error(error_info);

		return false;
	}

	//获取设置配置项值的命令
	if(getConfValueByLabelAndKey(softwareName, "setCommand", value))
	{
		//获取配置项当前值，然后将值进行除以2操作
		int configValueNum = StrToInt(configValue);
		configValueNum /= 2;
		memset(configValue, 0, CONFIG_VALUE_MAX_NUM);
		IntToStr(configValue, configValueNum);
		sprintf(setCommand, value, configName, configValue);
	}
	else
	{
		WriteLog(0, ERROR_LOG_FILE, "调用者信息\n", file, function, line);
		sprintf(error_info, "label:%s key:setCommand failure.\n", softwareName);
		Error(error_info);

		return false;
	}

	//在线修改配置项值
	return ExecuteCommand(setCommand);
}
