/**************************************
*
* 名称:      string头文件
* 功能：     提供字符串函数接口的调用
* 创建人：   代振全
* 当前版本:  v1.0
* 修改记录:  2017-08-11 创建
*
**************************************/
#ifndef __FTP_STRING_H__
#define __FTP_STRING_H__



int string_SendData(int nSockFd, void *vBuf, int nbytes);
int string_RecvData(int nSockFd , char *pBuf, int nbytes);
int string_ReplaceStrFromOldStrToNewStr(char *pStr, char *pOldStr, char *pNewStr, int nMaxLen);


#endif