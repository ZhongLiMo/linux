#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include <string>

#include "tcppacket.h"

class TCPClient
{
public:
	TCPClient(int fd, const std::string& ip);
	int sendToClient();
public:
    int m_fd;
	std::string m_ip;
	TCPPacket tcppacket;
	friend class TCPServer;
};


#endif  //!_TCP_CLIENT_H_