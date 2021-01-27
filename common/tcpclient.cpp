#include "tcpclient.h"
#include "netbase.h"


TCPClient::TCPClient(int fd) : m_fd(fd)
{

}

int TCPClient::sendToServer()
{
    if (tcppacket.safe_check() != 0)
    {
        return -1;
    }
    fd_write(m_fd, reinterpret_cast<char*>(&tcppacket.header), TCP_HEAD_SIZE);
    if (tcppacket.header.length > 0)
    {
        fd_write(m_fd, reinterpret_cast<char*>(&tcppacket.buffer), tcppacket.header.length);
    }
    return 0;
}