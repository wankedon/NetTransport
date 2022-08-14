#include <iostream>
#include "NetTransport.h"
#include <vector>
#include <windows.h>

void receiveHandler(const DataBlock& db, void* user)
{
    std::vector<char> data(db.length);
    memcpy(data.data(), db.addr, db.length);
    for (size_t i = 0; i < db.length; i++)
    {
        std::cout << "receive:" << data[i] - '0' << std::endl;
    }
};

void udpClient()
{
    TransportCallBacks trcbs;
    trcbs.receive = receiveHandler;
    int fd;
    CreateTransport(&fd, WorkMode::UDP, trcbs);
    SetLocalPort(fd, 5555);
    SetDestEndpoint(fd, "127.0.0.1:4444");
    Start(fd);
    int i = 0;
    while (true)
    {
        Sleep(1000);
        std::vector<char> data(1, (char)(i + 48));
        DataBlock db{ data.data(),data.size() };
        Send(fd, db);
        i++;
    }
    //关闭通信
    CloseTransport(fd);
}

void tcpClient()
{
    TransportCallBacks trcbs;
    trcbs.receive = receiveHandler;
    int fd;
	CreateTransport(&fd, WorkMode::TCP_CLIENT, trcbs);
	SetDestEndpoint(fd, "127.0.0.1:4444");
	Start(fd);
	int i = 0;
	while (true)
	{
		Sleep(1000);
		std::vector<char> data(1, (char)(i + 48));
        DataBlock db{ data.data(),data.size() };
        Send(fd, db);
		i++;
	}
}

int main()
{
    //初始化库
    InitLib();
    //udpClient();
    tcpClient();
    std::system("pause");
    //关闭库
    DeInitLib();
}