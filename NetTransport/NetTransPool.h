#pragma once
class NetTrans;
class NetTransPool
{
public:
	NetTransPool();
	~NetTransPool();

public:
	std::pair<int, NET_Error> newHandle(const WorkMode workMode, const TransportCallBacks& trcbs);
	NET_Error closeHandle(int handle);
	std::shared_ptr<NetTrans> getTransport(int handle);
private:
	std::map<int, std::shared_ptr<NetTrans>> m_socketsInUse; ///<在用的DeviceSession
	std::list<int> m_Handles;		    ///句柄池
	const int MAX_HANDLE_COUNT = 64;	///句柄数量
	const int MAGIC_NUM = 0xCE41;	    ///句柄标识生成
};

