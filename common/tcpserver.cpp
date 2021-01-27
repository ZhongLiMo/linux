#include "tcpserver.h"

#include "netbase.h"
#include "tcpclient.h"
#include "tcppacket.h"

TCPServer::TCPServer(int(*client_process)(TCPClient*)) : client_process(client_process)
{
}

int TCPServer::initServer()
{
    m_fd = listen_ip_port("127.0.0.1", "6389");
    if (m_fd <= 0)
    {
        return -1;
    }
    m_eplfd = epoll_create_fd(MAX_FD_SZIE);
    if (m_fd <= 0)
    {
        fd_close(m_fd);
        return -1;
    }
    epoll_add(m_eplfd, m_fd, new TCPClient(m_fd));
    return 0;
}

int TCPServer::onProcess()
{
	static TCPClient* client = NULL;
    int fds = epoll_wait(m_eplfd, m_events, MAX_FD_SZIE, 0);
    if (fds > 0)
    {
        int i = 0;
        for (i = 0; i < fds; i++)
        {
			client = (TCPClient*)(m_events[i].data.ptr);
            if (client == NULL)
            {
                //�ҵ��������Ҳ��Ϊ��
            }
            else if (client->fd() == m_fd)
            {
                acceptClient();
            }
            else
            {
                int iRet = recvClient(client);
                if (iRet < 0)
                {
                    closeClient(client);
                }
            }
        }
    }
	return 0;
}

int TCPServer::acceptClient()
{
	static struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	socklen_t len = sizeof(addr);
	int fd = accept(m_fd, (struct sockaddr*)&addr, &len);
	if (fd <= 0)
	{
		return -1;
	}
	if (set_socket(fd) < 0)
	{
		fd_close(fd);
		return -1;
	}
	TCPClient* client = new TCPClient(fd);
	epoll_add(m_eplfd, fd, client);
	m_clientset.insert(client);
	return 0;
}

int TCPServer::closeClient(TCPClient* pClient)
{
	std::set<TCPClient*>::iterator ite = m_clientset.find(pClient);
	if (ite != m_clientset.end())
	{
		m_clientset.erase(ite);
	}
	if (pClient == NULL)
	{
		return -1;
	}
	epoll_del(m_eplfd, pClient->fd());
	close(pClient->fd());
	delete pClient;
	return 0;
}

int TCPServer::recvClient(TCPClient* pClient)
{
	int result = 0;
	if (pClient->tcppacket.cursize < TCP_HEAD_SIZE)
	{
		result = recv(pClient->fd(), &pClient->tcppacket.header, TCP_HEAD_SIZE - pClient->tcppacket.cursize, 0);
	}
	else if (pClient->tcppacket.cursize == TCP_HEAD_SIZE && pClient->tcppacket.header.length == 0)
	{
		goto ON_RECV_MSG;
	}
	else
	{
		result = recv(pClient->fd(), pClient->tcppacket.buffer, TCP_BUFF_SIZE - pClient->tcppacket.cursize - TCP_HEAD_SIZE, 0);
	}

	if (result == -1)
	{
		if (errno == EAGAIN || errno == EINTR || errno == EINPROGRESS || errno == EWOULDBLOCK)
		{
			return 0;
		}
		closeClient(pClient);
	}
	else if (result == 0)
	{
		closeClient(pClient);
	}
	else
	{
		pClient->tcppacket.cursize += result;
		if (pClient->tcppacket.cursize == (pClient->tcppacket.header.length + TCP_HEAD_SIZE))
		{
		ON_RECV_MSG:
			if (client_process(pClient) < 0)
			{
				return -1;
			}
		}
	}
	return 0;
}