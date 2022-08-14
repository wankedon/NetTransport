#pragma once                                                                                                        
#include "NetBase.h"

class TCPClient :public NetBase
{
public:
	TCPClient(const std::string destEndpoint, const TransportCallBacks& tcbs);
	~TCPClient();

public:
    bool start() override;
    void stop() override;
    void send(const std::vector<char>& buffer) override;

private:    
    void connect(tcp::resolver::iterator endpoint_iterator);
    void doWrite();
    void doReceive();
    void closeSocket();

private:
    bool m_connect;
    tcp::socket m_socket;
    std::string m_destEndpoint;
    boost::asio::ip::basic_resolver_results< boost::asio::ip::tcp> m_endpoint;
};