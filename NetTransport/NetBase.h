#pragma once
#include "NetTransport.h"
#include <boost/asio.hpp> 
using namespace boost;
using boost::asio::ip::tcp;

using PackageToSend = std::list<std::vector<char>>;
class RepeatedTimer;
class NetBase
{
public:
	NetBase(const TransportCallBacks& tcbs);
	virtual ~NetBase();

public:
	virtual bool start() = 0;
	virtual void stop() = 0;
	virtual void send(const std::vector<char>& buffer) = 0;
	void startTimer(const uint64_t timeout_ms);
	void setReceiveBufferSize(const size_t length) { m_receiveBuf.resize(length); }

protected:
	asio::io_service m_io_service;
	asio::io_context m_context;
	std::vector<char> m_receiveBuf;
	PackageToSend m_packagesToSend;
	PackageToSend m_packagesBuffer;
	ReceiveHandler* m_dataHandler;
	TransportCallBacks m_tcbs;
	std::future<void> m_future;
	std::mutex m_mutex;
	std::shared_ptr<RepeatedTimer> m_timer;
};