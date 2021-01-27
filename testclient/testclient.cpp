#include <iostream>  
#include "tcppacket.h"
#include "tcpclient.h"
#include "netbase.h"
#include "net.h"
#include <stdio.h>
#include <iostream>

#define SERVER_ADDRESS      "127.0.0.1"  
#define SERVER_PORT         (6388)  

#define SOCKET_NUM          120       /// 客户端socket的个数,修改该值可以改变连接到服务器的客户端个数  


char g_szBuf[512] = "";


int main()
{
	int iSock;
    struct sockaddr_in stSvrAddr;
    int iRet;

    iSock =  create_socket();
    if (iSock <= 0)
    {
        printf("error: socket()\n");
        return iSock;
    }

    bzero(&stSvrAddr, sizeof(stSvrAddr));
    stSvrAddr.sin_family = AF_INET;
    stSvrAddr.sin_port = htons(SERVER_PORT);
    stSvrAddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    iRet = connect(iSock, (struct sockaddr *)&stSvrAddr, sizeof(stSvrAddr));
    if (iRet < 0)
    {
        printf("error: connect()\n");
        close(iSock);
        return -1;
    }

	TCPClient client(iSock);

	while (true)
	{
		char szInput[256] = "";
		printf("Me: cmd mg\n");
		unsigned int cmd = 0;
		scanf("%d %[^\n]", &cmd, szInput);
		if (strcmp(szInput, "exit") == 0)
		{
			break;
		}
		else
		{
			client.tcppacket.pack_packet(szInput, sizeof(szInput), cmd);
			client.sendToServer();
		}

		if (fd_read(iSock, szInput, sizeof(szInput)))
		{
			printf("recv %s\n", szInput);
		}
	}

	fd_close(iSock);

	return 0;
}