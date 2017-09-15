#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "ftp_socket.h"



/**************************************
*设置sock连接为非阻塞模式
*成功返回0，失败返回-1 
**************************************/
int socket_SetNonBlocking(const int nSockFd)
{
	int nOpt = -1;

	
	if (nOpt = fcntl(nSockFd, F_GETFL) < 0)
	{
		printf("ngtSetNonBlocking fcntl F_GETFL is failure!\n");
		return -1;
	}
	
	nOpt = (nOpt | O_NONBLOCK);
	
	if (fcntl(nSockFd, F_SETFL, nOpt) < 0)
	{
		printf("ngtSetNonBlocking fcntl F_SETFL is failure!\n");
		return -1;
	}
	
	return 0;
}



/****************************************
*初始化服务器的sock套接字
*成功返回监听套接字，失败返回-1
*****************************************/
int socket_ServerInit(const char *pServerIP, const int nPort)
{
	int nSockFd = 0;
	int nOpt = 1;
	struct sockaddr_in struServerSock;

	printf("nPort:[%d]\n", nPort);
	
	bzero(&struServerSock, sizeof(struServerSock));
	
	struServerSock.sin_family = AF_INET;
	
	if (pServerIP == NULL)
		struServerSock.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		struServerSock.sin_addr.s_addr = inet_addr(pServerIP);
	
	struServerSock.sin_port = htons(nPort);
	
	if ((nSockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("ngtServerInit socket is failure!\n");
		return -1;
	}
#if 0
	if (socket_SetNonBlocking(nSockFd) == -1)
	{
		printf("ngtServerInit ngtSetNonBlocking is failure!\n");
		close(nSockFd);
		return -1;
	}
#endif
	if (setsockopt(nSockFd, SOL_SOCKET, SO_REUSEADDR, &nOpt, sizeof(nOpt)) == -1)
	{
		printf("ngtServerInit setsockopt is failure!\n");
		close(nSockFd);
		return -1;
	}
	
	if (bind(nSockFd, (struct sockaddr *)&struServerSock, sizeof(struServerSock)) == -1)
	{
		printf("ngtServerInit bind is failure!\n");
		close(nSockFd);
		return -1;
	}
	
	if (listen(nSockFd, BACK_LOG) == -1)
	{
		printf("ngtServerInit listen is failure!\n");
		close(nSockFd);
		return -1;
	}
	
	return nSockFd;
}



/****************************************
*连接到指定的服务器
*成功返回连接后新的套接字描述符,失败返回-1
*****************************************/
int socket_ConnectServer(const char *pServIP, const int nPort)
{
	int nSockFd = 0;
	struct sockaddr_in struServSock;

	bzero(&struServSock, sizeof(struServSock));

	struServSock.sin_family = AF_INET;
	inet_pton(AF_INET, pServIP, &struServSock.sin_addr);
	struServSock.sin_port = htons(nPort);

	if ((nSockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket_ConnectServer socket is failure!\n");
		return -1;
	}

	if (connect(nSockFd, (struct sockaddr *)&struServSock, sizeof(struServSock)) == -1)
	{
		printf("socket_ConnectServer connect is failure!\n");
		close(nSockFd);
		return -1;
	}

	return nSockFd;
}
