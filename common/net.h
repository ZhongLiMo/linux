#ifndef _NET_H_
#define _NET_H_

#include <sys/socket.h>		//socket
#include <netinet/in.h>		//socket sockaddr_in
#include <arpa/inet.h>		//inet_addr inet_aton
#include <fcntl.h>			//fcntl
#include <unistd.h>			//close
#include <sys/epoll.h>		//epoll
#include <string.h>			//memset
#include <errno.h>			//errno


const int MAX_FD_SIZE = 10000;
const int LISTEN_FD_SIZE = 128;
const int TCP_BUFF_SIZE = 1024 * 100;
const int SNDBUF_RCVBUF_SIZE = 1024 * 32;

#endif // !_NET_H_