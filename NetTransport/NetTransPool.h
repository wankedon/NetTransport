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
	std::map<int, std::shared_ptr<NetTrans>> m_socketsInUse; ///<���õ�DeviceSession
	std::list<int> m_Handles;		    ///�����
	const int MAX_HANDLE_COUNT = 64;	///�������
	const int MAGIC_NUM = 0xCE41;	    ///�����ʶ����
};

