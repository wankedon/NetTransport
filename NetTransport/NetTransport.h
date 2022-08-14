#pragma once

#ifdef NETTRANSPORT_EXPORTS
#define NETTRANSPORT_API __declspec(dllexport)
#else
#define NETTRANSPORT_API __declspec(dllimport)
#endif
#include <stdint.h>

//错误码
enum NET_Error
{
	NET_ERR_NONE = 0,
	NET_ERR_INIT = -1,
	NET_ERR_INVALID_HANDLE = -2,
	NET_ERR_SEND = -3,
	NET_ERR_NO_HANDLE_RESOURCE = -4,
	NET_ERR_NO_SET_DESTENDPOINT = -5,
	NET_ERR_NO_SET_LOCALPORT = -6,
	NET_ERR_START = -7,
};

//工作模式
enum WorkMode
{
	UDP,
	TCP_CLIENT,
	TCP_SERVER
};

//发送接收数据块
struct DataBlock
{
	char* addr;
	size_t length;
};

//接收数据回调函数
typedef void ReceiveHandler(const DataBlock& db, void* user);
//定时操作回调函数
typedef void TimeOutHandler(void* user);
struct TransportCallBacks
{
	void* owner = nullptr;
	ReceiveHandler* receive = nullptr;
	TimeOutHandler* timeout = nullptr;
};

//初始化库
NETTRANSPORT_API NET_Error InitLib();
//创建通信
NETTRANSPORT_API NET_Error CreateTransport(int* fd, const WorkMode workMode, const TransportCallBacks& trcbs);
//设置目的地址
NETTRANSPORT_API NET_Error SetDestEndpoint(int fd, const char* destEndpoint);
//设置本地端口号
NETTRANSPORT_API NET_Error SetLocalPort(int fd, const unsigned short localPort);
//设置接收缓冲区大小,默认1024
NETTRANSPORT_API NET_Error SetReceiveBufferSize(int fd, const size_t length);
//启动
NETTRANSPORT_API NET_Error Start(int fd);
//发送数据
NETTRANSPORT_API NET_Error Send(int fd, const DataBlock& db);
//启动定时器
NETTRANSPORT_API NET_Error StartTimer(int fd, const uint64_t timeInterval);
//关闭通信
NETTRANSPORT_API NET_Error CloseTransport(int fd);
//关闭库
NETTRANSPORT_API NET_Error DeInitLib();