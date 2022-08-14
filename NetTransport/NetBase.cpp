#include "pch.h"
#include "NetBase.h"
#include "RepeatedTimer.h"

NetBase::NetBase(const TransportCallBacks& tcbs)
	:m_receiveBuf(1024),
	m_tcbs(tcbs),
	m_dataHandler(tcbs.receive)
{

}

NetBase::~NetBase()
{

}

void NetBase::startTimer(const uint64_t timeout_ms)
{
	m_timer = std::make_shared<RepeatedTimer>(m_io_service, m_tcbs);
	m_timer->Start(timeout_ms);
}

void NetBase::stop()
{
	if (m_timer)
	{
		m_timer->Stop();
	}
	if (!m_future.valid())
		return;
	m_io_service.stop();
	m_future.get();
}