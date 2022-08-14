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
        std::cout << "receive:" << data[i]/* - '0'*/ << std::endl;
    }
};

void udpServer()
{
    TransportCallBacks trcbs;
    trcbs.receive = receiveHandler;
    int fd;
    CreateTransport(&fd, WorkMode::UDP, trcbs);
    SetLocalPort(fd, 5151);
    SetDestEndpoint(fd, "127.0.0.1:5555");
    Start(fd);
    //std::system("pause");
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

void tcpServer()
{
    TransportCallBacks trcbs;
    trcbs.receive = receiveHandler;
    int fd;
    CreateTransport(&fd, WorkMode::TCP_SERVER, trcbs);
    SetLocalPort(fd, 5151);
    Start(fd);
   // std::system("pause");
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

int main()
{
    //初始化库
    InitLib();
    //udpServer();
    tcpServer();
    std::system("pause");
    //关闭库
    DeInitLib();
}