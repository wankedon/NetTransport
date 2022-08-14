#pragma once

#ifdef NETTRANSPORT_EXPORTS
#define NETTRANSPORT_API __declspec(dllexport)
#else
#define NETTRANSPORT_API __declspec(dllimport)
#endif
#include <stdint.h>

//������
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

//����ģʽ
enum WorkMode
{
	UDP,
	TCP_CLIENT,
	TCP_SERVER
};

//���ͽ������ݿ�
struct DataBlock
{
	char* addr;
	size_t length;
};

//�������ݻص�����
typedef void ReceiveHandler(const DataBlock& db, void* user);
//��ʱ�����ص�����
typedef void TimeOutHandler(void* user);
struct TransportCallBacks
{
	void* owner = nullptr;
	ReceiveHandler* receive = nullptr;
	TimeOutHandler* timeout = nullptr;
};

//��ʼ����
NETTRANSPORT_API NET_Error InitLib();
//����ͨ��
NETTRANSPORT_API NET_Error CreateTransport(int* fd, const WorkMode workMode, const TransportCallBacks& trcbs);
//����Ŀ�ĵ�ַ
NETTRANSPORT_API NET_Error SetDestEndpoint(int fd, const char* destEndpoint);
//���ñ��ض˿ں�
NETTRANSPORT_API NET_Error SetLocalPort(int fd, const unsigned short localPort);
//���ý��ջ�������С,Ĭ��1024
NETTRANSPORT_API NET_Error SetReceiveBufferSize(int fd, const size_t length);
//����
NETTRANSPORT_API NET_Error Start(int fd);
//��������
NETTRANSPORT_API NET_Error Send(int fd, const DataBlock& db);
//������ʱ��
NETTRANSPORT_API NET_Error StartTimer(int fd, const uint64_t timeInterval);
//�ر�ͨ��
NETTRANSPORT_API NET_Error CloseTransport(int fd);
//�رտ�
NETTRANSPORT_API NET_Error DeInitLib();