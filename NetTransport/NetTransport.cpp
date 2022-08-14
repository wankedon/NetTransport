#include "pch.h"
#include "NetTransport.h"
#include "NetTrans.h"
#include "NetTransPool.h"

std::unique_ptr<NetTransPool> netTransPool;
NETTRANSPORT_API NET_Error InitLib()
{
	if (netTransPool == nullptr)
	{
		netTransPool = std::make_unique<NetTransPool>();
	}
	return NET_ERR_NONE;
}

NETTRANSPORT_API NET_Error CreateTransport(int* fd, const WorkMode workMode, const TransportCallBacks& trcbs)
{
	if (netTransPool == nullptr)
		return NET_ERR_INIT;
	auto pair = netTransPool->newHandle(workMode, trcbs);
	*fd = pair.first;
	return pair.second;
}

NETTRANSPORT_API NET_Error SetDestEndpoint(int fd, const char* destEndpoint)
{
	if (netTransPool == nullptr)
		return NET_ERR_INIT;
	auto transport = netTransPool->getTransport(fd);
	if (transport == nullptr)
		return NET_ERR_INVALID_HANDLE;
	transport->setDestEndpoint(destEndpoint);
	return NET_ERR_NONE;
}

NETTRANSPORT_API NET_Error SetLocalPort(int fd, const unsigned short localPort)
{
	if (netTransPool == nullptr)
		return NET_ERR_INIT;
	auto transport = netTransPool->getTransport(fd);
	if (transport == nullptr)
		return NET_ERR_INVALID_HANDLE;
	transport->setLocalPort(localPort);
	return NET_ERR_NONE;
}

NETTRANSPORT_API NET_Error SetReceiveBufferSize(int fd, const size_t length)
{
	if (netTransPool == nullptr)
		return NET_ERR_INIT;
	auto transport = netTransPool->getTransport(fd);
	if (transport == nullptr)
		return NET_ERR_INVALID_HANDLE;
	transport->setReceiveBufferSize(length);
	return NET_ERR_NONE;
}

NETTRANSPORT_API NET_Error Start(int fd)
{
	if (netTransPool == nullptr)
		return NET_ERR_INIT;
	auto transport = netTransPool->getTransport(fd);
	if (transport == nullptr)
		return NET_ERR_INVALID_HANDLE;
	transport->start();
	return NET_ERR_NONE;
}

NETTRANSPORT_API NET_Error Send(int fd,const DataBlock& db)
{
	if (netTransPool == nullptr)
		return NET_ERR_INIT;
	auto transport = netTransPool->getTransport(fd);
	if (transport == nullptr)
		return NET_ERR_INVALID_HANDLE;
	transport->send(db);
	return NET_ERR_NONE;
}

NETTRANSPORT_API NET_Error StartTimer(int fd, const uint64_t timeInterval)
{
	if (netTransPool == nullptr)
		return NET_ERR_INIT;
	auto transport = netTransPool->getTransport(fd);
	if (transport == nullptr)
		return NET_ERR_INVALID_HANDLE;
	transport->startTimer(timeInterval);
	return NET_ERR_NONE;
}

NETTRANSPORT_API NET_Error CloseTransport(int fd)
{
	if (netTransPool == nullptr)
		return NET_ERR_INIT;
	if (fd == -1)
		return NET_ERR_INVALID_HANDLE;
	return netTransPool->closeHandle(fd);
}

NETTRANSPORT_API NET_Error DeInitLib()
{
	netTransPool.reset();
	return NET_ERR_NONE;
}