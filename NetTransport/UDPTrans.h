#pragma once
#include "NetBase.h"

class UDPTrans :public NetBase
{
public:
	UDPTrans(const std::string destEndpoint, const unsigned short localPort, const TransportCallBacks& tcbs);
	~UDPTrans();

public:
	bool start() override;
	void stop() override;
	void send(const std::vector<char>& data) override;

private:
	void sendBuffer(const asio::const_buffer& buffer);
	void doReceive();
	void onSendComplete(std::error_code ec, std::size_t bytes_sent);

private:
	bool m_isHasPair;
	asio::ip::udp::socket m_socket;
	asio::ip::udp::endpoint m_dest_endpoint;
	asio::ip::udp::endpoint m_remote_endpoint;
};