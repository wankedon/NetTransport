#include "pch.h"
#include "NetTrans.h"
#include "UDPTrans.h"
#include "TCPClient.h"
#include "TCPServer.h"

NetTrans::NetTrans(const WorkMode workMode, const TransportCallBacks& trcbs)
	:m_workMode(workMode),
	m_trcbs(trcbs),
	m_destEndpoint("0.0.0.0:0000"),
	m_localPort(0)
{

}

NetTrans::~NetTrans()
{

}

NET_Error NetTrans::setDestEndpoint(const char* destEndpoint)
{
	m_destEndpoint = destEndpoint;
	return NET_ERR_NONE;
}

NET_Error NetTrans::setLocalPort(const unsigned short localPort)
{
	m_localPort = localPort;
	return NET_ERR_NONE;
}

NET_Error NetTrans::start()
{
	if (m_net == nullptr)
	{
		auto erro = creatNet();
		if (erro != NET_ERR_NONE)
		{
			return erro;
		}
		else
		{
			return m_net->start() ? NET_ERR_NONE : NET_ERR_START;
		}
	}
	else
	{
		return m_net->start() ? NET_ERR_NONE : NET_ERR_START;
	}
}

NET_Error NetTrans::creatNet()
{
	switch (m_workMode)
	{
	case UDP:
		if (m_localPort == 0 || m_destEndpoint == "0.0.0.0:0000")
			return NET_ERR_NO_SET_LOCALPORT;
		m_net = std::make_unique<UDPTrans>(m_destEndpoint, m_localPort, m_trcbs);//"127.0.0.1:5050", 5151, 
		break;
	case TCP_CLIENT:
		if (m_destEndpoint == "0.0.0.0:0000")
			return NET_ERR_NO_SET_DESTENDPOINT;
		m_net = std::make_unique<TCPClient>(m_destEndpoint, m_trcbs);
		break;
	case TCP_SERVER:
		if (m_localPort == 0)
			return NET_ERR_NO_SET_LOCALPORT;
		m_net = std::make_unique<TCPServer>(m_localPort, m_trcbs);
		break;
	default:
		break;
	}
	return NET_ERR_NONE;
}

NET_Error NetTrans::stop()
{
	if (m_net)
		m_net->stop();
	return NET_ERR_NONE;
}

NET_Error NetTrans::setReceiveBufferSize(const size_t length)
{
	if (m_net)
		m_net->setReceiveBufferSize(length);
	return NET_ERR_NONE;
}

NET_Error NetTrans::send(const DataBlock& db)
{
	if (m_net)
	{
		std::vector<char> buffer(db.length);
		memcpy(buffer.data(), db.addr, db.length);
		m_net->send(buffer);
	}
	return NET_ERR_NONE;
}

NET_Error NetTrans::startTimer(const uint64_t timeout_ms)
{
	if (m_net)
		m_net->startTimer(timeout_ms);
	return NET_ERR_NONE;
}