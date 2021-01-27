#include "tcppacket.h"

int TCPPacket::safe_check()
{
	if (cursize != (unsigned int)header.length)
	{
		return -1;
	}
	return header.cmd;
}

int TCPPacket::save_packet(TCPHeader* h, char* b, unsigned short size)
{
	if (NULL == h || NULL == b || size > TCP_BUFF_SIZE)
	{
		return -1;
	}
	cursize = size;
	memcpy(buffer, b, size);
	memcpy(&header, h, TCP_HEAD_SIZE);
	return safe_check();
}

int TCPPacket::pack_packet(char* b, unsigned short size, unsigned int cmd, unsigned int uid, short ver)
{
	if (NULL == b || size > TCP_BUFF_SIZE)
	{
		return -1;
	}
	cursize = size;
	memcpy(buffer, b, size);
	header.cmd = cmd;
	header.uid = uid;
	header.version = ver;
	header.length = size;
	return safe_check();
}