#include "onlinemain.h"

#include "tcpclient.h"
#include "cscmd.h"
#include "mysqltable.h"

extern MyLog mainlog;

int deal_client_msg(TCPClient* pTCPClient, TCPPacket* pTCPPacket)
{
    if (pTCPPacket->safe_check() < 0)
    {
        mainlog.SaveLog(LOG_ERROR, "fd[%d] ip[%s] body[%s]", pTCPClient->m_fd, pTCPClient->m_ip.c_str());
        return -1;
    }
    switch (pTCPPacket->header.cmd)
    {
        case CMD_C_REGISTER_USER:
        {
            RegisterUser user;
            memcpy(&user, pTCPPacket->buffer, 20);
            for (auto ite = userTable.begin(); ite != userTable.end(); ++ite)
            {
                if (strcmp(ite->second->GetString(USER_TABLE_NAME).c_str(), user.name) == 0)
                {
                    //已有用户
                    printf("c_str[%s] user.name[%s]", ite->second->GetString(USER_TABLE_NAME).c_str(), user.name);
                    return -1;
                }
            }
            PUserRecord pUserRecord = UserRecord::CreateNew(userTable.GetNewKey());
            pUserRecord->SetString(USER_TABLE_NAME, std::string(user.name));
            userTable.InsertRecord(pUserRecord, true);
        }
        break;
        case CMD_C_TALK:
        {
            RegisterUser user;
            memcpy(&user, pTCPPacket->buffer, 20);
            PTalkRecord pTalkRecord = TalkRecord::CreateNew(talkTable.GetNewKey());
            pTalkRecord->SetString(USER_TALK_CONTENT, std::string(user.name));
            talkTable.InsertRecord(pTalkRecord, false);
        }
        break;
        default:
        {
            pTCPClient->sendToClient();
            mainlog.SaveLog(LOG_ERROR, "fd[%d] ip[%s] send wrong cmd[%d]", pTCPClient->m_fd, pTCPClient->m_ip.c_str(), static_cast<int>(pTCPPacket->header.cmd));
            return -1;
        }
        break;
    }
    mainlog.SaveLog(LOG_DEBUG, "cmd:[%d] lenth[%d] body[%s]", static_cast<int>(pTCPPacket->header.cmd), static_cast<int>(pTCPPacket->header.length), pTCPPacket->buffer);
    pTCPClient->sendToClient();
    return 0;
}