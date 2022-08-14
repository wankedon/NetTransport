#include "pch.h"
#include "TCPClient.h"
#include <iostream>   

TCPClient::TCPClient(const std::string destEndpoint, const TransportCallBacks& tcbs)
	:NetBase(tcbs),
	m_destEndpoint(destEndpoint),
	m_socket(m_io_service),
	m_connect(false)
{
	std::size_t size = destEndpoint.find(":");
	tcp::resolver resolver(m_io_service);
	m_endpoint = resolver.resolve(destEndpoint.substr(0, size), destEndpoint.substr(size + 1, 4));
}

TCPClient::~TCPClient()
{
	stop();
}

bool TCPClient::start()
{
	if (m_future.valid())
		return m_connect;
	m_future = std::async([this] { connect(m_endpoint); m_io_service.run(); });
	return m_connect;
}

void TCPClient::stop()
{
	closeSocket();
	NetBase::stop();
}

void TCPClient::connect(tcp::resolver::iterator endpoint_iterator)
{
	auto handler = [this](boost::system::error_code ec, tcp::resolver::iterator it)
	{
		if (!ec)
		{
			m_connect = true;
			std::cout << "Connect To TCPServer!"<< std::endl;
			doReceive();
		}
		else
		{
			connect(m_endpoint);
			std::cout << "DisConnect To TCPServer " << m_destEndpoint <<std::endl;
			//std::cout << "packagesBuffer.size:" << m_packagesBuffer.size()<< std::endl;
		}
	};
	boost::asio::async_connect(m_socket, endpoint_iterator, handler);
}

void TCPClient::doReceive()
{
	auto buffer = boost::asio::buffer(m_receiveBuf.data(),1024);
	auto hanlder = [this](boost::system::error_code ec, std::size_t lenght)
	{
		if (!ec && lenght > 0)
		{
			if (m_dataHandler)
			{
				DataBlock db{ m_receiveBuf.data(),lenght };
				//m_dataHandler(db, this);
				m_tcbs.receive(db, m_tcbs.owner);
				//m_dataHandler(m_receiveBuf, lenght,);
			}
			doReceive();
		}
		else
		{
			m_socket.close();
		}
	};
	//boost::asio::async_read(m_socket,buffer, hanlder);//读取指定长度
	m_socket.async_read_some(buffer, hanlder);//有数据就读取
}

void TCPClient::send(const std::vector<char>& data)
{
	m_packagesBuffer.push_back(data);
	if (!m_connect)
		return;
	auto handler = [this, data]()
	{
		bool sendInProgress = !m_packagesToSend.empty();
		m_packagesToSend = std::move(m_packagesBuffer);
		//m_packagesToSend.push_back(data);
		if (!sendInProgress)
		{
			doWrite();
		}
	};
	m_io_service.post(handler);
}

void TCPClient::doWrite()
{
	auto buffer = boost::asio::buffer(m_packagesToSend.front().data(), m_packagesToSend.front().size());
	auto handler = [this](boost::system::error_code ec, std::size_t length)
	{
		if (!ec)
		{
			m_packagesToSend.pop_front();
			if (!m_packagesToSend.empty())
			{
				doWrite();
			}
		}
		else
		{
			m_socket.close();
		}
	};
	boost::asio::async_write(m_socket, buffer, handler);
}

void TCPClient::closeSocket()
{
	auto handler = [this]()
	{
		m_socket.close();
	};
	m_io_service.post(handler);
}