#pragma once
#include "NetBase.h"
#include "TCPSession.h"

class TCPServer :public NetBase
{
public:
    TCPServer(const unsigned short localPort, const TransportCallBacks& tcbs);
    ~TCPServer();
public:
    bool start() override;
    void stop() override;
    void send(const std::vector<char>& data) override;
private:
	void do_accept();
private:
    tcp::acceptor m_acceptor;
    std::shared_ptr<TCPSession> m_session;
};