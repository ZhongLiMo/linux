#include "onlinemain.h"

#include "tcpclient.h"

int deal_client_msg(TCPClient* pTCPClient)
{
    switch (pTCPClient->fd())
    {
    case 1:
        {
            //序列化协议
        }
        break;
    default:
        break;
    }
}


int main(int argc, char *argv[])
{
    
}