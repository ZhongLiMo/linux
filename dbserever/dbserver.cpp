#include "onlinemain.h"

#include <json/json.h>
#include <fstream>

#include "timer.h"
#include "tcpserver.h"
#include "mysqltable.h"
#include "mylog.h"

static const char* SERVER_CONFIGFILE = "../conf/server.conf";

TimerManager timerManager;
TCPServer tcpserver(deal_client_msg);
MysqlDB* mysqldb = DBHandle->GetInstance();

MyLog mainlog("DBSERVER", "../log");
MyLog mysqllog("MYSQL_DBSERVER", "../log");

int read_conf_file(Json::Value& root);

int main(int argc, char *argv[])
{
	Json::Value root;
	if (read_conf_file(root) < 0)
    {
        mainlog.SaveLog(LOG_FATAL, "read_conf_file[%s] error", SERVER_CONFIGFILE);
    }
    
    mysqldb->Connect(root["mysql"]["mysql_ip"].asString().c_str(), root["mysql"]["user_name"].asString().c_str(),
        root["mysql"]["password"].asString().c_str(), root["mysql"]["database"].asString().c_str());

    if (init_mysql_data() < 0)
    {
        mainlog.SaveLog(LOG_FATAL, "init_mysql_data error");
    }

    if (tcpserver.initServer(root["online"]["listen_ip"].asString().c_str(), root["online"]["listen_port"].asString().c_str()) < 0)
    {
        mainlog.SaveLog(LOG_FATAL, "initServer error");
    }

    timerManager.InitTimer(10);
    while (1)
    {
        //TCP消息
        tcpserver.onProcess();
        //定时器
        timerManager.OnTimer();
    }
    return 0;
}

int read_conf_file(Json::Value& root)
{
    std::ifstream is(SERVER_CONFIGFILE, std::ios::binary);
    if (!is.is_open())
    {
        return -1;
    }
	Json::Reader reader;
	if (!reader.parse(is, root))
	{
		is.close();
        return -1;
	}
	is.close();
    return 0;
}