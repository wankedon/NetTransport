#include "pch.h"
#include "UDPTrans.h"
#include <iostream>
using namespace std;

UDPTrans::UDPTrans(const std::string destEndpoint, const unsigned short localPort, const TransportCallBacks& tcbs)
	:NetBase(tcbs),
	m_socket(m_io_service, asio::ip::udp::endpoint(asio::ip::udp::v4(), localPort)),
	m_isHasPair(false)
{
	auto size = destEndpoint.find(":");
	auto ip = destEndpoint.substr(0, size);
	short port = atoi(destEndpoint.substr(size + 1, 4).c_str());
	m_dest_endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string(ip), port);
	std::cout << "UDPServer LocalPort: " << localPort << std::endl;
	std::cout << "DestEndpoint: " << destEndpoint << std::endl;
}

UDPTrans::~UDPTrans()
{
	stop();
}

void UDPTrans::doReceive()
{
	auto handler = [this](std::error_code ec, std::size_t bytes_recvd)
	{
		if (!ec && bytes_recvd > 0)
		{
			if (m_tcbs.receive)
			{
				DataBlock db{ m_receiveBuf.data(),bytes_recvd };
				m_tcbs.receive(db, m_tcbs.owner);
			}

			m_dest_endpoint = m_remote_endpoint;
		}
		else
		{
			m_isHasPair = false;
		}
		doReceive();
	};
	m_socket.async_receive_from(asio::buffer(m_receiveBuf), m_remote_endpoint, handler);
}

void UDPTrans::send(const std::vector<char>& data)
{
	m_packagesBuffer.push_back(data);
	sendBuffer(asio::buffer(data));
	m_isHasPair = true;
}

void UDPTrans::sendBuffer(const asio::const_buffer& buffer)
{
	auto* p = (const char*)buffer.data();
	std::vector<char> buf{ p, p + buffer.size() };
	std::lock_guard<mutex> lg(m_mutex);
	bool sendInProgress = !m_packagesToSend.empty();
	//m_packagesToSend.emplace_back(m_packagesBuffer.front());
	m_packagesToSend.emplace_back(std::move(buf));
	if (!sendInProgress)
	{
		//if (m_isHasPair)
		//{ //有对方 把积累数据放入队列
		//	m_packagesToSend = std::move(m_packagesBuffer);
		//	if (m_packagesToSend.size() > 1)
		//	{
		//		m_packagesToSend.pop_front();//长度大于1 说明有累积，则移除第一个已经发送过的数据
		//	}
		//}
		m_socket.async_send_to(
			asio::buffer(m_packagesToSend.front()), m_dest_endpoint,
			[this](std::error_code ec, std::size_t bytes_sent)
			{
				onSendComplete(ec, bytes_sent);
			});
	}
}

void UDPTrans::onSendComplete(std::error_code ec, std::size_t bytes_sent)
{
	std::lock_guard<mutex> lg(m_mutex);
	assert(!m_packagesToSend.empty());
	m_packagesToSend.pop_front();
	if (!m_packagesToSend.empty())
	{
		auto& head = m_packagesToSend.front();
		m_socket.async_send_to(
			asio::buffer(head), m_dest_endpoint,
			[this](std::error_code ec, std::size_t bytes_sent)
			{
				onSendComplete(ec, bytes_sent);
			});
	}
}

bool UDPTrans::start()
{
	if (m_future.valid())
		return false;
	m_future = std::async([this] {doReceive(); m_io_service.run(); });
	return true;
}

void UDPTrans::stop()
{
	NetBase::stop();
}

namespace boost
{
	void throw_exception(std::exception const& e)
	{
		return;
	}
}