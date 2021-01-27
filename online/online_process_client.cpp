#include "onlinemain.h"

#include "tcpclient.h"
#include "cscmd.h"
#include "mysqldb.h"
#include "all_table_date.h"

extern DBTble<UserRecord> userTable;

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
                if (strcmp(ite->GetString(USER_TABLE_NAME).c_str(), user.name) == 0)
                {
                    //已有用户
                    return -1;
                }
            }
            std::shared_ptr<Record> pUserRecord = UserRecord::CreateNew(DBTble<UserRecord>::GetNewKey());
            pUserRecord->SetString(USER_TABLE_NAME, std::strng(user.name));
            userTable.InsertRecord(pUserRecord);
        }
        break;
    default:
        break;
    }
}