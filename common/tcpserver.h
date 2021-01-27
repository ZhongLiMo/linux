#ifndef _SERVER_H_
#define _SERVER_H_

class TCPClient;

#include "net.h"

#include <set>

class TCPServer
{
public:
	TCPServer(int(*client_process)(TCPClient*));
    int initServer();
    int onProcess();
private:
    int acceptClient();
    int closeClient(TCPClient* pClient);
    int recvClient(TCPClient* pClient);
private:
	int m_fd;
	int m_eplfd;
    std::set<TCPClient*> m_clientset;
    struct epoll_event m_events[MAX_FD_SZIE];
	int(*client_process)(TCPClient*);
};

#endif  //!_SERVER_H_