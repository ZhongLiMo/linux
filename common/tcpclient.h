#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "tcppacket.h"

class TCPClient
{
public:
	TCPClient(int fd);
	int fd() { return m_fd; }
	TCPPacket tcppacket;
private:
    int m_fd;
};


#endif  //!_TCP_CLIENT_H_