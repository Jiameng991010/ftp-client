/**************************************
*
* 名称:      command头文件
* 功能：     提供指令函数接口的调用
* 创建人：   代振全
* 当前版本:  v1.0
* 修改记录:  2017-08-11 创建
*
**************************************/
#ifndef __FTP_COMMAND_H__
#define __FTP_COMMAND_H__



int command_SendCommand(int nSockFd, const char *pCmd, const char *pBuf, int nFlag);
int command_UserLogin(const int nSockFd, char *pUserName, char *pUserPasswd);
int command_UserQuit(const int nSockFd);
int command_SetTransType(const int nSockFd, char zTransType);
int command_SetTransPort(const int nSockFd, FtpManagerInfo *struFtpManagerInfo);
int command_GetList(const int nSockFd);
int command_GetCd(const int nSockFd, const char *pDirPath);
int command_GetPwd(const int nSockFd);
int command_GetSyst(const int nSockFd);
int command_GetFile(const int nSockFd, const char *pFileName);
int command_PutFile(const int nSockFd, const char *pFileName);



#endif