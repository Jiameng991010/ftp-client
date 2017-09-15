#include <stdio.h>
#include <string.h>

#include "ftp_common.h"
#include "ftp_socket.h"
#include "ftp_command.h"


int main(int argc, char *argv[])
{
	int nRetValue = -1;
	int nSockFd = 0;
	char szCmdLine[128] = {0};
	char szBuffer[4096] = {0};
	char szSplitLint[5][20] = {0};

	nSockFd = socket_ConnectServer(SERVER_IP, SERVER_PORT);
	
	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
    if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);


	nRetValue = command_UserLogin(nSockFd, USER_NAME, USER_PASSWD);
	if (nRetValue == -1)
	{
		DEBUG("登陆失败!\n");
		return -1;
	}

	while (1)
	{
		DEBUG(">");
		fgets(szCmdLine, 128, stdin);
		szCmdLine[strlen(szCmdLine) - 1] = '\0';

		DEBUG("szCmdLine:[%s]\n", szCmdLine);
		sscanf(szCmdLine, "%[^ ] %[^ ] %[^ ] %s", &szSplitLint[0], &szSplitLint[1], &szSplitLint[2], &szSplitLint[3]);

		DEBUG("szSplitLint[0]:[%s]\n", szSplitLint[0]);
		DEBUG("szSplitLint[1]:[%s]\n", szSplitLint[1]);
		DEBUG("szSplitLint[2]:[%s]\n", szSplitLint[2]);
		DEBUG("szSplitLint[3]:[%s]\n", szSplitLint[3]);
		
		if (strncmp("quit", szCmdLine, 4) == 0)
		{
			DEBUG("退出\n");
			nRetValue = command_UserQuit(nSockFd);
			DEBUG("nRetValue: [%d]\n", nRetValue);
			break;
		}
		else if ((strncmp("?", szCmdLine, 1) == 0) || (strncmp("help", szCmdLine, 4) == 0))
		{
			DEBUG("显示帮助\n");
		}
		else if (strncmp("syst", szCmdLine, 4) == 0)
		{
			DEBUG("获取服务器使用的操作系统\n");
			nRetValue = command_GetSyst(nSockFd);
			DEBUG("nRetValue: [%d]\n", nRetValue);
		}
		else if (strncmp("type", szCmdLine, 4) == 0)
		{
			DEBUG("设置传输类型\n");
			nRetValue = command_SetTransType(nSockFd, 'I');
			DEBUG("nRetValue: [%d]\n", nRetValue);
		}
		else if (strncmp("pwd", szCmdLine, 3) == 0)
		{
			DEBUG("显示当前工作目录\n");
			nRetValue = command_GetPwd(nSockFd);
			DEBUG("nRetValue: [%d]\n", nRetValue);
		}
		else if (strncmp("cd", szCmdLine, 2) == 0)
		{
			DEBUG("改变服务器上的工作目录\n");
			nRetValue = command_GetCd(nSockFd, szSplitLint[1]);
			DEBUG("nRetValue: [%d]\n", nRetValue);
		}
		else if (strncmp("port", szCmdLine, 4) == 0)
		{
			DEBUG("IP地址和两字节的端口ID\n");
			//nRetValue = command_SetTransPort(nSockFd);
			//DEBUG("nRetValue: [%d]\n", nRetValue);
		}
		else if ((strncmp("ls", szCmdLine, 2) == 0) || (strncmp("dir", szCmdLine, 3) == 0))
		{
			DEBUG("列出当前目录的文件名\n");
			nRetValue = command_GetList(nSockFd);
			DEBUG("nRetValue: [%d]\n", nRetValue);
		}
		else if (strncmp("get", szCmdLine, 3) == 0)
		{
			DEBUG("从服务器获取文件\n");
			nRetValue = command_GetFile(nSockFd, szSplitLint[1]);
			DEBUG("nRetValue: [%d]\n", nRetValue);
		}
		else if (strncmp("put", szCmdLine, 3) == 0)
		{
			DEBUG("向服务器发送文件\n");
			nRetValue = command_PutFile(nSockFd, szSplitLint[1]);
			DEBUG("nRetValue: [%d]\n", nRetValue);
		}
		memset(szCmdLine, 0, sizeof(szCmdLine));
	}

	close(nSockFd);
	
	return 0;
}