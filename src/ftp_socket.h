/**************************************
*
* 名称:      socket头文件
* 功能：     提供socket函数接口的调用
* 创建人：   代振全
* 当前版本:  v1.0
* 修改记录:  2017-08-10 创建
*
**************************************/

#ifndef __NGT_SOCKET_H__
#define __NGT_SOCKET_H__


/*监听套接字排队等待的个数*/
#define BACK_LOG 5





int socket_SetNonBlocking(const int nSockFd);
int socket_ServerInit(const char *pServerIP, const int nPort);
int socket_ConnectServer(const char *pServIP, const int nPort);


#endif