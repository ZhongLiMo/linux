#include "onlinemain.h"

#include "timer.h"
#include "tcpserver.h"
#include "mysqldb.h"


MysqlDB mysqldb;
TCPServer tcpserver;
TimerManager timer_manager;

DBTble<UserRecord> userTable;						//完整的表
char userTableName[] = "test_user";

int init_mysql_data();

int main(int argc, char *argv[])
{
    tcpserver.initServer("127.0.0.1", 6388);
    timer_manager.InitTimer(10);

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