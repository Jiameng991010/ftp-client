/**************************************
*
* 名称:      common头文件
* 功能：     提供common函数接口的调用
* 创建人：   代振全
* 当前版本:  v1.0
* 修改记录:  2017-08-10 创建
*
**************************************/
#ifndef __FTP_COMMON_H__
#define __FTP_COMMON_H__


#define SERVER_IP			"127.0.0.1"
#define SERVER_PORT			21
#define USER_NAME			"xiaodai"
#define USER_PASSWD			"xiaodai"

#define MAX_INPUT_SIZE		256


/*自定义打印调式printf宏*/
#define DEBUG(format, args... ) { \
	printf("[Info][%s:%s line:%d] " format, __FILE__, __FUNCTION__, __LINE__, ##args); }



typedef struct {
	int nServerSockFd;
	int nDataSockFd;
}FtpManagerInfo;





#endif
