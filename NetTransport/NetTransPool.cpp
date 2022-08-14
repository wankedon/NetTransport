#include "pch.h"
#include "NetTransport.h"
#include "NetTransPool.h"
#include "NetTrans.h"

using namespace std;

NetTransPool::NetTransPool()
{
	for (auto i = 0; i < MAX_HANDLE_COUNT; i++)
	{
		m_Handles.push_back(MAGIC_NUM + i);   ///构造函数中构建MAX_HANDLE_COUNT个备用句柄
	}
}


NetTransPool::~NetTransPool()
{
}

/**
* @brief 创建新的连接
* @param[in] peerAddress 远端主机地址
* @param[in] handler 回调函数集合
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
* @brief 获得DeviceSession的一个实例
* @param[in] handle PositionSocket句柄
* @return shared_ptr<DeviceSession>
* @retval DeviceSession实例的指针
*/
std::shared_ptr<NetTrans> NetTransPool::getTransport(int handle)
{
	if (m_socketsInUse.find(handle) == m_socketsInUse.end()) 
		return nullptr;
	return m_socketsInUse[handle];
}

/**
* @brief 关闭句柄
* @details
* @param[int] handle PositionSocket句柄
* @return PS_Error
*/
NET_Error NetTransPool::closeHandle(int handle)
{
	if (m_socketsInUse.find(handle) == m_socketsInUse.end())
		return NET_ERR_INVALID_HANDLE;
	m_socketsInUse[handle]->stop();
	m_socketsInUse.erase(handle);			///< 排除socket，自动清理
	m_Handles.push_back(handle);		    ///< 归还句柄号
	return NET_ERR_NONE;
}