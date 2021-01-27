#include "netbase.h"

#include <stdlib.h>
#include <stdio.h>

#include "net.h"

int set_nonblock(int fd)
{
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

int set_send_buf(int fd)
{
	int on = SNDBUF_RCVBUF_SIZE;
    return setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &on, sizeof(on));
}

int set_recv_buf(int fd)
{
	int on = SNDBUF_RCVBUF_SIZE;
    return setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &on, sizeof(on));
}

int set_reuse(int fd)
{
	int on = 1;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
}

int set_keepllive(int fd)
{
	int on = 1;
	return setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on));
}

int set_socket(int fd)
{
	if (set_nonblock(fd) < 0 || set_send_buf(fd) < 0 || set_recv_buf(fd) < 0 
		|| set_reuse(fd) < 0 || set_keepllive(fd) < 0)
	{
		return -1;
	}
	return 0;
}

int create_socket()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd <= 0)
	{
		return -1;
	}
	if (set_socket(fd) < 0)
	{
		return -1;
	}
	return fd;
}

int listen_ip_port(const char* sIP, const char* sPort)
{
	if (sIP == NULL || sPort == NULL)
	{
		return -1;
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(static_cast<uint16_t>(atoi(sPort)));
	addr.sin_addr.s_addr = inet_addr(sIP);

	int fd = create_socket();
	if (fd <= 0)
	{
		perror("socket");
		return -1;
	}

	if (bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) != 0)
	{
		perror("bind");
		close(fd);
		return -1;
	}

	if (listen(fd, LISTEN_FD_SIZE) != 0)
	{
		perror("listen");
		close(fd);
		return -1;
	}
	return fd;
}

int epoll_create_fd(int size)
{
    return epoll_create(size);
}

int epoll_add(int epollfd, int fd, void *ptr)
{
	static struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.ptr = ptr;
	return epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

int epoll_del(int epollfd, int fd)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
    return 0;
}

int fd_close(int fd)
{
	close(fd);
	return 0;
}

int fd_read(int sockfd, char *buf, size_t count)
{
	size_t bytes_read = 0;
    int this_read;

    while (bytes_read < count)
    {
        do
        {
            this_read = read(sockfd, buf, count - bytes_read);
        }
        while ((this_read < 0) && (errno == EINTR));

        if (this_read < 0)
        {
            if (bytes_read > 0 && errno == EAGAIN)
            {
                return bytes_read;
            }
            else
            {
                return this_read;
            }
        }
        else if (this_read == 0)
        {
            return bytes_read;
        }

        bytes_read += this_read;
        buf += this_read;
    }

    return count;
}

int fd_write(int sockfd, char *buf, size_t count)
{
	size_t bytes_sent = 0;
    int this_write;

    while (bytes_sent < count)
    {
        do
        {
            this_write = write(sockfd, buf, count - bytes_sent);
        }
        while ( (this_write < 0) && (errno == EINTR) );
        if (this_write <= 0)
        {
            return this_write;
        }
        bytes_sent += this_write;
        buf += this_write;
    }
    return count;
}