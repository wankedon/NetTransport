#include "pch.h"
#include "NetTransport.h"
#include "NetTransPool.h"
#include "NetTrans.h"

using namespace std;

NetTransPool::NetTransPool()
{
	for (auto i = 0; i < MAX_HANDLE_COUNT; i++)
	{
		m_Handles.push_back(MAGIC_NUM + i);   ///���캯���й���MAX_HANDLE_COUNT�����þ��
	}
}


NetTransPool::~NetTransPool()
{
}

/**
* @brief �����µ�����
* @param[in] peerAddress Զ��������ַ
* @param[in] handler �ص���������
* @return pair<int, PS_Error>
*/

std::pair<int, NET_Error> NetTransPool::newHandle(const WorkMode workMode, const TransportCallBacks& trcbs)
{
	if (m_Handles.empty())
		return std::pair<int, NET_Error>{-1, NET_ERR_NO_HANDLE_RESOURCE};
	auto trans = make_shared<NetTrans>(workMode, trcbs);
	int handle = m_Handles.front();
	assert(m_socketsInUse.find(handle) == m_socketsInUse.end());
	m_socketsInUse[handle] = trans;
	m_Handles.pop_front();
	return std::pair<int, NET_Error>{handle, NET_ERR_NONE};
}

/**
* @brief ���DeviceSession��һ��ʵ��
* @param[in] handle PositionSocket���
* @return shared_ptr<DeviceSession>
* @retval DeviceSessionʵ����ָ��
*/
std::shared_ptr<NetTrans> NetTransPool::getTransport(int handle)
{
	if (m_socketsInUse.find(handle) == m_socketsInUse.end()) 
		return nullptr;
	return m_socketsInUse[handle];
}

/**
* @brief �رվ��
* @details
* @param[int] handle PositionSocket���
* @return PS_Error
*/
NET_Error NetTransPool::closeHandle(int handle)
{
	if (m_socketsInUse.find(handle) == m_socketsInUse.end())
		return NET_ERR_INVALID_HANDLE;
	m_socketsInUse[handle]->stop();
	m_socketsInUse.erase(handle);			///< �ų�socket���Զ�����
	m_Handles.push_back(handle);		    ///< �黹�����
	return NET_ERR_NONE;
}