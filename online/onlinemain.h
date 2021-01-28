#ifndef _ONLINE_MAIN_H_
#define _ONLINE_MAIN_H_

class TCPClient;
class TCPPacket;

int deal_client_msg(TCPClient* pTCPClient, TCPPacket* pTCPPacket);
int main(int argc, char *argv[]);

#endif // !_ONLINE_MAIN_H_