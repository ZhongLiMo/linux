#include "onlinemain.h"

#include "tcpclient.h"
#include "cscmd.h"
#include "mysqldb.h"
#include "mysqltable.h"

#include "mysqldb.cpp"


extern char userTableName[];



typedef DBRecord<UserTableField, USER_TABLE_MAX, userTableName> UserRecord;
typedef DBTble<UserRecord> UserTable;

extern UserTable userTable;

int init_mysql_data()
{
    if (!UserRecord::InitDefaultRecord() || !DBHandle->Select(userTable, userTableName))
    {
        return -1;
    }
    return 0;
}



int deal_client_msg(TCPClient* pTCPClient, TCPPacket* pTCPPacket)
{
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
            std::shared_ptr<UserRecord> pUserRecord = UserRecord::CreateNew(userTable.GetNewKey());
            pUserRecord->SetString(USER_TABLE_NAME, std::string(user.name));
            userTable.InsertRecord(pUserRecord, true);
        }
        break;
    case CMD_C_TALK:
        {

        }
        break;
    default:
        printf("cmd:[%d] lenth[%d] body[%s]", pTCPPacket->header.cmd, pTCPPacket->header.length, pTCPPacket->buffer);
        pTCPClient->sendToClient();
        break;
    }
    return 0;
}