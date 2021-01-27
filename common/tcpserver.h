#ifndef _SERVER_H_
#define _SERVER_H_

class TCPClient;
class TCPPacket;

#include "net.h"

#include <set>

class TCPServer
{
public:
	TCPServer(int(*clinetProcess)(TCPClient*, TCPPacket*));
    virtual ~TCPServer();
public:
    int initServer(const char* ip, const char* port);
    int onProcess();
private:
    int acceptClient();
    int closeClient(TCPClient* pClient);
    int recvClient(TCPClient* pClient);
private:
	int m_fd;
	int m_eplfd;
    std::set<TCPClient*> m_clientset;
    struct epoll_event m_events[MAX_FD_SIZE];
	int(*clinetProcess)(TCPClient*, TCPPacket*);
};

#endif  //!_SERVER_H_