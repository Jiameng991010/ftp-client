#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "ftp_common.h"
#include "ftp_string.h"
#include "ftp_command.h"




/**************************************
*向FTP服务器发送数据包
*成功返回0，失败返回-1 
**************************************/
int command_SendCommand(int nSockFd, const char *pCmd, const char *pBuf, int nFlag)
{
	int 		nRetValue = -1;
	char 		szRequest[MAX_INPUT_SIZE];


	memset(szRequest, 0, sizeof(szRequest));

	if (nFlag == 0)
		sprintf(szRequest, "%s\r\n", pCmd);
	else
		sprintf(szRequest, "%s %s\r\n", pCmd, pBuf);

	DEBUG("ReqBytes:[%d],Request:[%s]\n", strlen(szRequest), szRequest);
	
	nRetValue = string_SendData(nSockFd, szRequest, strlen(szRequest));
	if (nRetValue == -1)
		return -1;

	return 0;
}



/**************************************
*登陆FTP服务器
*成功返回0，失败返回-1 
**************************************/
int command_UserLogin(const int nSockFd, char *pUserName, char *pUserPasswd)
{
	int 		nRetValue = -1;
	char 		szBuffer[1024];


	memset(szBuffer, 0, sizeof(szBuffer));

	if ((nSockFd <= 0) || (pUserName == NULL) || (pUserPasswd == NULL) 
		|| (strlen(pUserName) == 0) || (strlen(pUserPasswd) == 0))
		return -1;
	
	nRetValue = command_SendCommand(nSockFd, "USER", pUserName, 1);
	if (nRetValue == -1)
		return -1;
#if 0
	nSize = string_RecvData(nSockFd, szBuffer, sizeof(szBuffer));
#endif
	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);
	if (strncmp("331", szBuffer, 3) != 0)
		return -1;
	
	nRetValue = command_SendCommand(nSockFd, "PASS", pUserPasswd, 1);
	if (nRetValue == -1)
		return -1;

	memset(szBuffer, 0, sizeof(szBuffer));
	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);
	if (strncmp("230", szBuffer, 3) != 0)
		return -1;
	
	printf("登陆成功\n");
		
	return 0;
}


/**************************************
*从FTP服务器退出登陆
*成功返回0，失败返回-1 
**************************************/
int command_UserQuit(const int nSockFd)
{
	int 		nRetValue = -1;
	char 		szBuffer[1024];


	memset(szBuffer, 0, sizeof(szBuffer));

	nRetValue = command_SendCommand(nSockFd, "QUIT", "", 0);
	if (nRetValue == -1)
		return -1;

	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);

	return 0;
}



/**************************************
*设置传输类型
*成功返回0，失败返回-1 
**************************************/
int command_SetTransType(const int nSockFd, char zTransType)
{
	int 		nRetValue = -1;
	char 		szCmdType[10];
	char 		szBuffer[256];


	memset(szCmdType, 0, sizeof(szCmdType));
	memset(szBuffer, 0, sizeof(szBuffer));
	
	sprintf(szCmdType, "TYPE %c", zTransType);

	nRetValue = command_SendCommand(nSockFd, szCmdType, NULL, 0);
	if (nRetValue == -1)
		return -1;

	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;

	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);
	if (strncmp("200", szBuffer, 3) != 0)
		return -1;
	
	return 0;
}



/**************************************
*设置数据传输端口
*成功返回0，失败返回-1 
**************************************/
int command_SetTransPort(const int nSockFd, FtpManagerInfo *struFtpManagerInfo)
{
	int 		nRetValue = -1;
	int 		nLocalPort = -1;
	int 		nDataSockFd = -1;
	int 		nServerSockFd = -1;
	char 		*pLocalIP = NULL;
	char 		szClientPost[8];
	char 		szPortAddr[36];
	char 		szBuffer[128];
	struct sockaddr_in struLocalAddr, struDataAddr;
	
	
	socklen_t sockLocalLen = sizeof(struLocalAddr);

	memset(szClientPost, 0, sizeof(szClientPost));
	memset(szPortAddr, 0, sizeof(szPortAddr));
	memset(szBuffer, 0, sizeof(szBuffer));
	memset(&struLocalAddr, 0, sizeof(struLocalAddr));
	
	if (getsockname(nSockFd, (struct sockaddr *)&struLocalAddr, &sockLocalLen) != 0)
	{
		printf("command_SetTransPort getsockname is failure!\n");
		return -1;
	}

	pLocalIP = inet_ntoa(struLocalAddr.sin_addr);
	nLocalPort = struLocalAddr.sin_port;

	DEBUG("LocalIP:[%s],LocalPort:[%d]\n", pLocalIP, nLocalPort);
	
	sprintf(szClientPost, "%d.%d", (int)(nLocalPort / 256), (int)(1 + nLocalPort - 256 * (int)(nLocalPort / 256)));
	sprintf(szPortAddr, "%s.%s", pLocalIP, szClientPost);

	nRetValue = string_ReplaceStrFromOldStrToNewStr(szPortAddr, ".", ",", strlen(szPortAddr));
	DEBUG("PortAddr:[%s]\n", szPortAddr);
	
	nServerSockFd = socket_ServerInit(pLocalIP, nLocalPort + 1);
	if (nServerSockFd == -1)
		return -1;

	nRetValue = command_SendCommand(nSockFd, "PORT", szPortAddr, 1);
	if (nRetValue == -1)
		return -1;

	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);
	if (strncmp("200", szBuffer, 3) != 0)
		return -1;
	
	DEBUG("发送端口成功!\n");

	//command_SendCommand(nSockFd, "LIST", "", 0);
	
	struFtpManagerInfo->nServerSockFd = nServerSockFd;
	
#if 0
	socklen_t sockDataLen = sizeof(struDataAddr);
	memset(&struDataAddr, 0, sizeof(struDataAddr));

	nDataSockFd = accept(nServerSockFd, (struct sockaddr *)&struDataAddr, &sockDataLen);
	DEBUG("有新的连接[%d]到来!\n", nDataSockFd);
#endif
	return nServerSockFd;
}



/**************************************
*从服务器获取文件列表信息
*成功返回0，失败返回-1 
**************************************/
int command_GetList(const int nSockFd)
{
	int 		nRetValue = -1;
	int 		nDataSockFd = -1;
	char		szBuffer[128];
	char 		szDataBuffer[1024];
	struct 		sockaddr_in struDataAddr;


	FtpManagerInfo 	struFtpManagerInfo;

	socklen_t 		sockDataLen = sizeof(struDataAddr);

	memset(szBuffer, 0, sizeof(szBuffer));
	memset(szDataBuffer, 0, sizeof(szDataBuffer));
	memset(&struDataAddr, 0, sizeof(struDataAddr));

	nRetValue = command_SetTransPort(nSockFd, &struFtpManagerInfo);
	if (nRetValue == -1)
		return -1;

	nRetValue = command_SendCommand(nSockFd, "LIST", "", 0);
	if (nRetValue == -1)
		return -1;

	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);

	nDataSockFd = accept(struFtpManagerInfo.nServerSockFd, (struct sockaddr *)&struDataAddr, &sockDataLen);
	DEBUG("有新的连接[%d]到来!\n", nDataSockFd);

	while ((nRetValue = read(nDataSockFd, szDataBuffer, sizeof(szDataBuffer))) > 0)
	{
		DEBUG("\n[%s]\n", szDataBuffer);
	}
	
	close(nDataSockFd);
	close(struFtpManagerInfo.nServerSockFd);

	memset(szBuffer, 0, sizeof(szBuffer));
	
	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);

	return 0;
}


/**************************************
*改变服务器上的工作目录
*成功返回0，失败返回-1 
**************************************/
int command_GetCd(const int nSockFd, const char *pDirPath)
{
	int 		nRetValue = -1;
	char 		szBuffer[128];


	memset(szBuffer, 0, sizeof(szBuffer));
	
	nRetValue = command_SendCommand(nSockFd, "CWD", pDirPath, 1);
	if (nRetValue == -1)
		return -1;

	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);
	
	return 0;
}


/**************************************
*显示服务器的当前工作目录
*成功返回0，失败返回-1 
**************************************/
int command_GetPwd(const int nSockFd)
{
	int 		nRetValue = -1;
	char 		szBuffer[128];


	memset(szBuffer, 0, sizeof(szBuffer));

	nRetValue = command_SendCommand(nSockFd, "PWD", "", 0);
	if (nRetValue == -1)
		return -1;

	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);
	
	return 0;
}


/**************************************
*获取服务器使用的操作系统
*成功返回0，失败返回-1 
**************************************/
int command_GetSyst(const int nSockFd)
{	
	int 		nRetValue = -1;
	char		szBuffer[1024];


	memset(szBuffer, 0, sizeof(szBuffer));

	nRetValue = command_SendCommand(nSockFd, "SYST", "", 0);
	if (nRetValue == -1)
		return -1;

	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);

	return 0;
}


/**************************************
*从服务器获取文件
*成功返回0，失败返回-1 
**************************************/
int command_GetFile(const int nSockFd, const char *pFileName)
{
	int 		nRetValue = -1;
	int 		nDataSockFd = -1;
	int 		nFileOpen = 0;
	int 		nRecvBytes = 0;
	FILE 		*outFile = NULL;
	char 		szBuffer[128];
	char 		szDataBuffer[1024];
	struct 		sockaddr_in struDataAddr;


	FtpManagerInfo 	struFtpManagerInfo;

	socklen_t 		sockDataLen = sizeof(struDataAddr);


	memset(szBuffer, 0, sizeof(szBuffer));
	memset(szDataBuffer, 0, sizeof(szDataBuffer));
	memset(&struDataAddr, 0, sizeof(struDataAddr));

	/*1 首先创建一个端口发送给FTP服务器*/
	nRetValue = command_SetTransPort(nSockFd, &struFtpManagerInfo);
	if (nRetValue == -1)
		return -1;

	/*2 向FTP服务器发送下载文件命令*/
	nRetValue = command_SendCommand(nSockFd, "RETR", pFileName, 1);
	if (nRetValue == -1)
		return -1;

	/*3 接收FTP服务器返回的指令*/
	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);
	if (strncmp("150", szBuffer, 3) != 0)
	{
		close(struFtpManagerInfo.nServerSockFd);
		printf("服务器不存在该文件[%s]或服务器端权限不够!\n", pFileName);
		return -1;
	}
	
	/*4 监听是否有新的客户端连接*/
	nDataSockFd = accept(struFtpManagerInfo.nServerSockFd, (struct sockaddr *)&struDataAddr, &sockDataLen);
	//DEBUG("有新的连接[%d]到来!\n", nDataSockFd);

	/*5 读取服务器发送过来的文件内容*/
	while ((nRetValue = read(nDataSockFd, szDataBuffer, sizeof(szDataBuffer))) > 0)
	{
		if (nFileOpen == 0)
		{
			if ((outFile = fopen(pFileName, "w")) == 0)
			{
				DEBUG("command_GetFile fopen file is failure!\n");
				close(nDataSockFd);
				return -1;
			}
			nFileOpen = 1;
		}
		//DEBUG("nRetValue:[%d]\n", nRetValue);
		//DEBUG("%s\n", szDataBuffer);
		write(fileno(outFile), szDataBuffer, nRetValue);
		nRecvBytes += nRetValue;
	}

	if (nFileOpen != 0)
		fclose(outFile);
	
	close(nDataSockFd);
	close(struFtpManagerInfo.nServerSockFd);

	memset(szBuffer, 0, sizeof(szBuffer));

	/*6 接收服务器21端口发送过来的返回指令*/
	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);

	DEBUG("%d bytes get.\n", nRecvBytes);
	
	return 0;
}


/**************************************
*上传文件到服务器
*成功返回0，失败返回-1 
**************************************/
int command_PutFile(const int nSockFd, const char *pFileName)
{
	int 		nRetValue = -1;
	int 		nDataSockFd = -1;
	int 		nSendBytes = 0;
	FILE 		*inFile = NULL;
	char 		szBuffer[128];
	char 		szDataBuffer[1024];
	struct 		sockaddr_in struDataAddr;


	FtpManagerInfo 	struFtpManagerInfo;

	socklen_t 		sockDataLen = sizeof(struDataAddr);


	memset(szBuffer, 0, sizeof(szBuffer));
	memset(szDataBuffer, 0, sizeof(szDataBuffer));
	memset(&struDataAddr, 0, sizeof(struDataAddr));


	/*1 首先打开将要发送的文件*/
	inFile = fopen(pFileName, "r");
	if (inFile == NULL)
		return -1;

	/*2 创建一个端口发送给FTP服务器*/
	nRetValue = command_SetTransPort(nSockFd, &struFtpManagerInfo);
	if (nRetValue == -1)
		return -1;

	/*3 向FTP服务器发送下载文件命令*/
	nRetValue = command_SendCommand(nSockFd, "STOR", pFileName, 1);
	if (nRetValue == -1)
		return -1;

	/*4 接收FTP服务器返回的指令*/
	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);
	if (strncmp("150", szBuffer, 3) != 0)
	{
		fclose(inFile);
		close(struFtpManagerInfo.nServerSockFd);
		printf("服务器已存在该文件[%s]或服务器端权限不够!\n", pFileName);
		return -1;
	}

	/*5 监听是否有新的客户端连接*/
	nDataSockFd = accept(struFtpManagerInfo.nServerSockFd, (struct sockaddr *)&struDataAddr, &sockDataLen);
	DEBUG("有新的连接[%d]到来!\n", nDataSockFd);

	/*6 读取服务器发送过来的文件内容*/
	while ((nRetValue = read(fileno(inFile), szDataBuffer, sizeof(szDataBuffer))) > 0)
	{
#if 0
		if (TYPE == TYPE_A)
		{
			string_ReplaceStrFromOldStrToNewStr((char *)szDataBuffer, "\r\n", "\n", 1024-1);
			string_ReplaceStrFromOldStrToNewStr((char *)szDataBuffer, "\n", "\r\n", 1024-1);
			write(nDataSockFd, (const char *)szDataBuffer, strlen((const char *)szDataBuffer));
		}
		else if (TYPE == TYPE_I)
		{
			write(nDataSockFd, (const char *)szDataBuffer, nRetValue);
		}
#endif
		write(nDataSockFd, (const char *)szDataBuffer, nRetValue);
		nSendBytes += nRetValue;

		memset(szDataBuffer, 0, sizeof(szDataBuffer));
	}
	
	memset(szDataBuffer, 0, sizeof(szDataBuffer));
	
	fclose(inFile);
	close(nDataSockFd);
	close(struFtpManagerInfo.nServerSockFd);

	memset(szBuffer, 0, sizeof(szBuffer));

	/*6 接收服务器21端口发送过来的返回指令*/
	nRetValue = read(nSockFd, szBuffer, sizeof(szBuffer));
	if (nRetValue == -1)
		return -1;
	
	szBuffer[nRetValue - 2] = '\0';
	DEBUG("ResBytes:[%d],Response:[%s]\n", nRetValue, szBuffer);

	DEBUG("%d bytes get.\n", nSendBytes);
	
	return 0;
}

