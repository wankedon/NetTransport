#pragma once
#include "NetTransport.h"
#include "NetBase.h"

class NetBase;
class NetTrans
{

public:
	NetTrans(const WorkMode workMode, const TransportCallBacks& trcbs);
	~NetTrans();

public:
	NET_Error setDestEndpoint(const char* destEndpoint);
	NET_Error setLocalPort(const unsigned short localPort);
	NET_Error setReceiveBufferSize(const size_t length);
	NET_Error start();	
	NET_Error stop();
	NET_Error send(const DataBlock& db);
	NET_Error startTimer(const uint64_t timeout_ms);

private:
	NET_Error creatNet();

private:
	WorkMode m_workMode;
	TransportCallBacks m_trcbs;
	std::string m_destEndpoint;
	unsigned short m_localPort;
	std::unique_ptr<NetBase> m_net;
};