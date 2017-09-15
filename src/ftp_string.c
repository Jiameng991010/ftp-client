#include <stdio.h>
#include <string.h>

#include "ftp_common.h"
#include "ftp_string.h"




/**************************************
*向socket发送数据包
*nSockFd: socket描述符
*pBuf	: 要发送的数据
*nbytes : 发送数据包的大小
*成功返回发送的字节数，失败返回-1 
**************************************/
int string_SendData(int nSockFd, void *vBuf, int nbytes)
{
	int			nLeft = -1;
	int 		nWritten = -1;
	const char *ptr = (const char*)vBuf;

	nLeft = nbytes;
	while (nLeft > 0)
	{
		nWritten = write(nSockFd, ptr, nLeft);
		if (nWritten <= 0)
		{
			return(nWritten);		/* error */
		}

		nLeft -= nWritten;
		ptr   += nWritten;
	}
	
	return(nbytes - nLeft);
}



/**************************************
*从socket接收数据包
*nSockFd: socket描述符
*pBuf	: 要接收的数据
*nbytes : 接收数据包的大小
*成功返回接收的字节数，失败返回-1 
**************************************/
int string_RecvData(int nSockFd , char *pBuf, int nbytes)
{
	int 		nSize = -1;
	int 		nRecvByte = 0;
	int 		nWant = nbytes;
	
	
	while(nWant > 0)
	{
		DEBUG("111111111111111\n");
		nSize = read(nSockFd, pBuf, nWant);
		DEBUG("nSize:[%d]\n", nSize);
		if (nSize <= 0)
		{
			return nRecvByte;
		}
		nWant -= nSize;
		pBuf += nSize;
		nRecvByte += nSize;
	}

	return nRecvByte;
}



/**************************************
*从原字符串中将Old字符串替换成New字符串
*如:[10.0.0.65.234.217]
*得:[10,0,0,65,251,217]
*成功返回替换的个数，失败返回-1 
**************************************/
int string_ReplaceStrFromOldStrToNewStr(char *pStr, char *pOldStr, char *pNewStr, int nMaxLen)
{
	int 		i = 0;
	int 		nStrLen = 0;
	int 		nOldStrLen = 0;
	int 		nNewStrLen = 0;
	char 		*pFoo = NULL;
	char 		*pBar = pStr;

	
	if ((pStr == NULL) || (pOldStr == NULL) || (pNewStr == NULL))
		return -1;

	nStrLen = strlen(pStr);
	nOldStrLen = strlen(pOldStr);
	nNewStrLen = strlen(pNewStr);

	pFoo = strstr(pBar, pOldStr);
	
	while (pFoo)
	{
		pBar = pFoo + strlen(pNewStr);
		memmove(pBar, pFoo + strlen(pOldStr), strlen(pFoo + strlen(pOldStr)) + 1);
		memcpy(pFoo, pNewStr, strlen(pNewStr));
		i++;
		pFoo = strstr(pBar, pOldStr);
		nStrLen = strlen(pStr);
	}

	return i;
}
