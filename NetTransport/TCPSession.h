#pragma once
#include "NetBase.h"

class TCPSession : public std::enable_shared_from_this<TCPSession>
{
public:
    TCPSession(boost::asio::io_service& io_service, const TransportCallBacks& tcbs);
    ~TCPSession();
public:
    tcp::socket& socket();
    void receive();
    void send(const std::vector<char>& data);
private:
    void doReceive();
    void doWrite();
private:
    std::mutex m_mutex;
    tcp::socket m_socket;              
    boost::asio::io_service& m_io_service;
    std::vector<char> m_receiveBuf;
    PackageToSend m_packageToSend;
    ReceiveHandler* m_receiveHandler;
    TransportCallBacks m_tcbs;
};