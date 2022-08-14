#include "pch.h"
#include "TCPServer.h"
#include "RepeatedTimer.h"
#include <iostream> 

TCPServer::TCPServer(const unsigned short localPort, const TransportCallBacks& tcbs)
	:NetBase(tcbs),
	m_acceptor(m_io_service, tcp::endpoint(tcp::v4(), localPort))
{
	do_accept();
	std::cout << "TCPServer Listen: " << localPort << std::endl;
}

TCPServer::~TCPServer()
{
	stop();
}

bool TCPServer::start()
{
	if (m_future.valid())
		return false;
	m_future = std::async([this] {m_io_service.run(); });
	return true;
}

void TCPServer::stop()
{
	NetBase::stop();
}

void TCPServer::send(const std::vector<char>& data)
{
	if (m_session)
		m_session->send(data);
}

void TCPServer::do_accept()
{
	auto session = std::make_shared<TCPSession>(m_io_service, m_tcbs);
	auto handler = [this, session](boost::system::error_code ec)
	{
		if (!ec)
		{
			session->receive();
			m_session = session;
		}
		do_accept();
	};
	m_acceptor.async_accept(session->socket(), handler);
}