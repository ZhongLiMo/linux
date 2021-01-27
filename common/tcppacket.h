#ifndef _TCP_PACKET_H_
#define _TCP_PACKET_H_

#include "net.h"

#pragma pack(push)
#pragma pack(1)
struct TCPHeader
{
	unsigned int cmd;
	unsigned short version;
	unsigned int uid;
	unsigned short length;
};

const int TCP_HEAD_SIZE = sizeof(TCPHeader);

struct TCPPacket
{
	int safe_check();
	int save_packet(TCPHeader* h, char* b, unsigned short size);
	int pack_packet(char* b, unsigned short size, unsigned int cmd = 0, unsigned int uid = 0, short ver = 0);
	TCPHeader			header;
	unsigned short		cursize;
	char				buffer[TCP_BUFF_SIZE];
};
#pragma pack(pop)

#endif // !_TCP_PACKET_H_