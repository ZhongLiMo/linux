#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "tcppacket.h"

class TCPClient
{
public:
	TCPClient(int fd);
	int sendToServer();
public:
    int m_fd;
	TCPPacket tcppacket;
	friend class TCPServer;
};


#endif  //!_TCP_CLIENT_H_