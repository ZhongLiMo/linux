#include "onlinemain.h"

#include "tcpclient.h"
#include "cscmd.h"
#include "mysqldb.h"
#include "all_table_date.h"

extern RecordTable<UserRecord> userTable;		//完整的表

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
                    return -1;
                }
            }
            std::shared_ptr<UserRecord> pUserRecord = UserRecord::CreateNew(userTable.GetNewKey());
            pUserRecord->SetString(USER_TABLE_NAME, std::string(user.name));
            userTable.InsertRecord(pUserRecord);
        }
        break;
    default:
        break;
    }
    return 0;
}