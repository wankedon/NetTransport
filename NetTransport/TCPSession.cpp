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

//�첽��ȡ��Ա���ݹ�������Ϣͷ
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
    //boost::asio::async_read(m_socket,buffer, hanlder);//��ȡָ������
    m_socket.async_read_some(buffer, handler);//�����ݾͶ�ȡ
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

//����Ϣд���ͻ���
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