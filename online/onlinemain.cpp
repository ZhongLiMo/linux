#include "onlinemain.h"

#include "timer.h"
#include "tcpserver.h"
#include "mysqldb.h"
#include "all_table_date.h"


MysqlDB* mysqldb = DBHandle->GetInstance();
TCPServer tcpserver(deal_client_msg);
TimerManager timer_manager;



RecordTable<UserRecord> userTable;		//完整的表


int init_mysql_data();

int main(int argc, char *argv[])
{
    tcpserver.initServer("127.0.0.1", "6388");
    timer_manager.InitTimer(10);
    mysqldb->Connect("127.0.0.1", "root", "M:ppujV+u4wr", "test");

    if (init_mysql_data() < 0)
    {
        return -1;
    }

    while (1)
    {
        tcpserver.onProcess();
        timer_manager.OnTimer();
    }
    return 0;
}

int init_mysql_data()
{
    //get all mysql data
    DBHandle->Select(userTable, userTableName);
    return 0;
}