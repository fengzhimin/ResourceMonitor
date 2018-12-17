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
static char warning_info[200];
static char result_info[200];
static char lineData[LINE_CHAR_MAX_NUM];
static char subStr2[2][MAX_SUBSTR];

int ExecuteCommandDebug(char *command, const char *file, const char *function, const int line)
{
	int status;
	status = system(command);
	if(status == -1)
	{
		WriteLog(0, "调用者信息\n", file, function, line);
		sprintf(error_info, "system error(%s)\n", command);
		Error(error_info);
		return status;
	}
	else
	{
		int ret_status = WEXITSTATUS(status);
		if(WIFEXITED(status))
		{
			//执行command命令成功
			if(0 == ret_status || 1 == ret_status)
			{
				//执行脚本成功
				sprintf(result_info, "execute command(%s) success\n", command);
				Result(result_info);
			}
			else
			{
				//脚本执行失败
				WriteLog(0, "调用者信息\n", file, function, line);
				sprintf(error_info, "execute command(%s) failure, return value(%d)\n", command, ret_status);
				Error(error_info);
			}

			return ret_status;
		}
		else
		{
			//执行command命令出错
			WriteLog(0, "调用者信息\n", file, function, line);
			sprintf(error_info, "execute command(%s) error, error code(%d)\n", command, ret_status);
			Error(error_info);

			return -2;
		}
	}
}

bool ReduceConfDebug(char *softwareName, char *confName, const char *file, const char *function, const int line)
{
	char scriptPath[SCRIPT_PATH_MAX_LENGTH];
	sprintf(scriptPath, "%s/%sDown.sh", ResourceMonitor_Client_SOLUTION_PATH, softwareName);
	if((access(scriptPath, F_OK)) == -1)
	{
		WriteLog(0, "调用者信息\n", file, function, line);
		sprintf(error_info, "script(%s) is not existing\n", scriptPath);
		Error(error_info);

		return false;
	}

	char command[COMMAND_MAX_LENGTH] = {0};
	sprintf(command, "bash %s %s", scriptPath, confName);
	int status = ExecuteCommand(command);
	if(0 == status)
	{
		//增加配置项值成功
		sprintf(result_info, "Reduce config success(software:%s\tconfName:%s)\n", softwareName, confName);
		Result(result_info);
		return true;
	}
	else
	{
		WriteLog(0, "调用者信息\n", file, function, line);
		switch(status)
		{
		case 2:
			sprintf(error_info, "get the value of config failure(software:%s\tconfName:%s)\n", softwareName, confName);
			break;
		case 3:
			sprintf(error_info, "set the value of config failure(software:%s\tconfName:%s)\n", softwareName, confName);
			break;
		default:
			sprintf(error_info, "Reduce config failure(software:%s\tconfName:%s), error code(%d)\n", softwareName, confName, status);
		}
		Error(error_info);

		return false;
	}

}

int IncreaseConfDebug(char *softwareName, char *confName, char *increaseValue, char *defValue, const char *file, const char *function, const int line)
{
	char scriptPath[SCRIPT_PATH_MAX_LENGTH];
	sprintf(scriptPath, "%s/%sUp.sh", ResourceMonitor_Client_SOLUTION_PATH, softwareName);
	if((access(scriptPath, F_OK)) == -1)
	{
		WriteLog(0, "调用者信息\n", file, function, line);
		sprintf(error_info, "script(%s) is not existing\n", scriptPath);
		Error(error_info);

		return -1;
	}

	char command[COMMAND_MAX_LENGTH] = {0};
	sprintf(command, "bash %s %s %s %s", scriptPath, confName, increaseValue, defValue);
	int status = ExecuteCommand(command);
	if(0 == status)
	{
		//增加配置项值成功
		sprintf(result_info, "Increase config success(software:%s\tconfName:%s)\n", softwareName, confName);
		Result(result_info);
	}
	else if(1 == status)
	{
		//配置项值等于默认值
		sprintf(result_info, "config value is equal to default value(software:%s\tconfName:%s\tdefault value:%s)\n", softwareName, confName, defValue);
		Result(result_info);
	}
	else
	{
		WriteLog(0, "调用者信息\n", file, function, line);
		switch(status)
		{
		case 2:
			sprintf(error_info, "get the value of config failure(software:%s\tconfName:%s)\n", softwareName, confName);
			break;
		case 3:
			sprintf(error_info, "set the value of config failure(software:%s\tconfName:%s)\n", softwareName, confName);
			break;
		default:
			sprintf(error_info, "Increase config failure(software:%s\tconfName:%s), error code(%d)\n", softwareName, confName, status);
		}
		Error(error_info);
	}

	return status;
}

bool RecordTunedConfInfoDebug(char *softwareName, char *confName, char *resourceType, const char *file, const char *function, const int line)
{
	char confInfo[CONFIG_VALUE_MAX_NUM + CONFIG_LABEL_MAX_NUM] = {0};
	sprintf(confInfo, "%s/%s:%s", softwareName, resourceType, confName);
	if(!access(REDUCE_CONFIG_PATH, F_OK))
	{
		//存在REDUCE_CONFIG_PATH
		int fd = OpenFile(REDUCE_CONFIG_PATH, O_RDONLY);
		if(fd == -1)
		{
			WriteLog(0, "调用者信息\n", file, function, line);
			sprintf(error_info, "open file(%s) failure\n", REDUCE_CONFIG_PATH);
			Error(error_info);
			return false;
		}

		//加锁操作
		flock(fd, LOCK_EX);

		//查找文件中是否已经记录了调整的配置信息
		bool isExist = false;
		while(ReadLine(fd, lineData))
		{
			if(strcmp(lineData, confInfo) == 0)
			{
				isExist = true;
				break;
			}
		}

		if(!isExist)
			WriteLine(fd, confInfo);

		//加锁操作
		flock(fd, LOCK_UN);

		CloseFile(fd);
	}
	else
	{
		//不存在REDUCE_CONFIG_PATH
		int fd = OpenFile(REDUCE_CONFIG_PATH, O_WRONLY);
		if(fd == -1)
		{
			WriteLog(0, "调用者信息\n", file, function, line);
			sprintf(error_info, "open file(%s) failure\n", REDUCE_CONFIG_PATH);
			Error(error_info);
			return false;
		}

		WriteLine(fd, confInfo);

		CloseFile(fd);
	}

	return true;
}

bool AutoIncreaseConfDebug(const char *file, const char *function, const int line)
{
	if(!access(REDUCE_CONFIG_PATH, F_OK))
	{
		char confInfo[CONFIG_VALUE_MAX_NUM + CONFIG_LABEL_MAX_NUM] = {0};
		//存在REDUCE_CONFIG_PATH
		int fd = OpenFile(REDUCE_CONFIG_PATH, O_RDONLY);
		int newFd = OpenFile(TMP_REDUCE_CONFIG_PATH, O_WRONLY);
		if(fd == -1)
		{
			WriteLog(0, "调用者信息\n", file, function, line);
			sprintf(error_info, "open file(%s) failure\n", REDUCE_CONFIG_PATH);
			Error(error_info);
			return false;
		}
		
		//加锁操作
		flock(fd, LOCK_EX);

		if(newFd == -1)
		{
			WriteLog(0, "调用者信息\n", file, function, line);
			sprintf(error_info, "open file(%s) failure\n", TMP_REDUCE_CONFIG_PATH);
			Error(error_info);
			return false;
		}

		//查找文件中是否已经记录了调整的配置信息
		while(ReadLine(fd, lineData))
		{
			cutStrByLabel(lineData, ':', subStr2, 2);
			strcpy(label, subStr2[0]);
			strcpy(name, subStr2[1]);
			cutStrByLabel(label, '/', subStr2, 2);
			strcpy(softwareName, subStr2[0]);
			if(getConfValueByLabelAndKey(label, "increaseValue", increaseValue) && getConfValueByLabelAndKey(label, "defaultValue", defaultValue))
			{
				int status = IncreaseConf(softwareName, name, increaseValue, defaultValue);
				if(status == 0)
				{
					WriteLine(newFd, lineData);
				}
				else if(status == 1)
				{
					//跳过已经增加到默认值的配置项
					continue;
				}
				else
				{
					WriteLog(0, "调用者信息\n", file, function, line);
					sprintf(error_info, "Increase config failure(software:%s\tconfig:%s\tincreaseValue:%s\tdefaultValue:%s)\n", \
							softwareName, name, increaseValue, defaultValue);
					Error(error_info);

					WriteLine(newFd, lineData);
				}
			}
		}

		//加锁操作
		flock(fd, LOCK_UN);

		CloseFile(fd);
		CloseFile(newFd);

		//重新替换临时文件
		if(remove(REDUCE_CONFIG_PATH) != 0)
		{
			sprintf(error_info, "remove %s failure!(%s)\n", REDUCE_CONFIG_PATH, strerror(errno));
			Error(error_info);
		}
		//重新命名临时文件
		if(rename(TMP_REDUCE_CONFIG_PATH, REDUCE_CONFIG_PATH) != 0)
		{
			sprintf(error_info, "rename %s to %s failure!(%s)\n", TMP_REDUCE_CONFIG_PATH, REDUCE_CONFIG_PATH, strerror(errno));
			Error(error_info);
		}
	}

	return true;
}
