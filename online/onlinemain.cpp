#include "onlinemain.h"

#include "timer.h"
#include "tcpserver.h"
#include "mysqldb.h"
#include "mysqltable.h"


MysqlDB* mysqldb = DBHandle->GetInstance();
TCPServer tcpserver(deal_client_msg);
TimerManager timerManager;


int main(int argc, char *argv[])
{
    timerManager.InitTimer(10);
    mysqldb->Connect("127.0.0.1", "root", "M:ppujV+u4wr", "test");

    if (init_mysql_data() < 0)
    {
        return -1;
    }
    
    tcpserver.initServer("127.0.0.1", "6388");
    while (1)
    {
        tcpserver.onProcess();
        timerManager.OnTimer();
    }
    return 0;
}