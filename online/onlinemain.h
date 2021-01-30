#ifndef _ONLINE_MAIN_H_
#define _ONLINE_MAIN_H_

class TCPClient;
class TCPPacket;

int deal_client_msg(TCPClient* pTCPClient, TCPPacket* pTCPPacket);

#endif // !_ONLINE_MAIN_H_