#include "onlinemain.h"

#include <fstream>

#include "timer.h"
#include "tcpserver.h"
#include "mysqltable.h"
#include "mylog.h"

//#define LUA_CONFIGFILE		"lua\\lua\\lua.conf"

//#include <json.h>

MysqlDB* mysqldb = DBHandle->GetInstance();
TCPServer tcpserver(deal_client_msg);
TimerManager timerManager;

MyLog mainlog("MAIN", "../log");

int main(int argc, char *argv[])
{
    timerManager.InitTimer(10);
    mysqldb->Connect("127.0.0.1", "root", "123456", "test");

    if (init_mysql_data() < 0)
    {
        return -1;
    }


    // std::ifstream is(LUA_CONFIGFILE, std::ios::binary);
	// CHECKF(is.is_open());
	// Json::Reader reader;
	// Json::Value root;
	// int nRole = 0;
	// std::string strOccupation;
	// std::string strCamp;

	// if (reader.parse(is, root))
	// {
	// 	int path_size = root["path"].size();
	// 	for (int i = 0; i < path_size; ++i)
	// 	{
    //         mainlog.SaveLog(LOG_INFO, "  [%s]", root["path"][i].asString().c_str());
	// 	}
	// }
	// is.close();
	// return true;
    
    tcpserver.initServer("127.0.0.1", "6388");
    while (1)
    {
        //mainlog.SaveLog(LOG_INFO, "cscasc");
        //TCP消息
        tcpserver.onProcess();
        //定时器
        timerManager.OnTimer();
    }
    return 0;
}