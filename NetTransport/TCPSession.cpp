#include "pch.h"
#include "TCPSession.h"
#include <iostream> 

TCPSession::TCPSession(boost::asio::io_service& io_service, const TransportCallBacks& tcbs)
    :m_io_service(io_service),
	m_socket(io_service), 
    m_receiveBuf(1024),
	m_tcbs(tcbs)
{

}

TCPSession::~TCPSession()
{

}

tcp::socket& TCPSession::socket()
{
    return m_socket;
}

void TCPSession::receive()
{
    std::cout << "TCPClient Enter!" << std::endl;
    doReceive();
}

//异步读取成员传递过来的消息头
void TCPSession::doReceive()
{
    auto self(shared_from_this());
    auto buffer = boost::asio::buffer(m_receiveBuf.data(),1024);
    auto handler = [self](boost::system::error_code ec, std::size_t lenght)
    {
		if (!ec && lenght > 0)
		{
			if (self->m_tcbs.receive)
			{
				DataBlock db{ self->m_receiveBuf.data(),lenght };
				//self->m_receiveHandler(db, self.get());
				self->m_tcbs.receive(db, self->m_tcbs.owner);
				//self->m_receiveHandler(self->m_receiveBuf, lenght);
			}

			self->doReceive();
		}
        else
        {
            std::cout << "TCPClient Exit!" << std::endl;
        }
    };
    //boost::asio::async_read(m_socket,buffer, hanlder);//读取指定长度
    m_socket.async_read_some(buffer, handler);//有数据就读取
}

void TCPSession::send(const std::vector<char>& data)
{
	auto handler = [this, data]()
	{
		std::lock_guard<std::mutex> lg(m_mutex);
		bool sendInProgress = !m_packageToSend.empty();
		m_packageToSend.push_back(data);
		if (!sendInProgress)
		{
			doWrite();
		}
	};
	m_io_service.post(handler);
}

//将消息写给客户端
void TCPSession::doWrite()
{
	auto self(shared_from_this());
	auto buffer = boost::asio::buffer(m_packageToSend.front().data(), m_packageToSend.front().size());
	auto handler = [self](boost::system::error_code ec, std::size_t length)
	{
		if (!ec)
		{
			self->m_packageToSend.pop_front();
			if (!self->m_packageToSend.empty())
			{
				self->doWrite();
			}
		}
		else
		{
            std::cout << "TCPClient Exit!" << std::endl;
		}
	};
	boost::asio::async_write(m_socket, buffer, handler);
}