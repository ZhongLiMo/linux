#include "onlinemain.h"

#include <json/json.h>
#include <fstream>

#include "timer.h"
#include "tcpserver.h"
#include "mysqltable.h"
#include "mylog.h"

static const char* SERVER_CONFIGFILE = "../conf/server.conf";
static const char* SHM_CONFIGFILE = "../conf/shm.conf";

TimerManager timerManager;
TCPServer tcpserver(deal_client_msg);
MysqlDB* mysqldb = DBHandle->GetInstance();

MyLog mainlog("MAIN", "../log");
MyLog mysqllog("MYSQL_MAIN", "../log");

int read_conf_file(Json::Value& root, const char* conf_file);
int dbProcess(const char(&strsql)[SQL_SIZE]);

int shm_key = 0;

int main(int argc, char *argv[])
{
	Json::Value root;
	if (read_conf_file(root, SERVER_CONFIGFILE) < 0)
    {
        mainlog.SaveLog(LOG_FATAL, "read_conf_file[%s] error", SERVER_CONFIGFILE);
    }
    
    mysqldb->Connect(root["mysql"]["mysql_ip"].asString().c_str(), root["mysql"]["user_name"].asString().c_str(),
        root["mysql"]["password"].asString().c_str(), root["mysql"]["database"].asString().c_str());

    mysqldb->InitDBProcess(dbProcess);

    if (init_mysql_data() < 0)
    {
        mainlog.SaveLog(LOG_FATAL, "init_mysql_data error");
    }

    if (tcpserver.initServer(root["online"]["listen_ip"].asString().c_str(), root["online"]["listen_port"].asString().c_str()) < 0)
    {
        mainlog.SaveLog(LOG_FATAL, "initServer error");
    }

    if (read_conf_file(root, SHM_CONFIGFILE) < 0)
    {
        mainlog.SaveLog(LOG_FATAL, "read_conf_file[%s] error", SERVER_CONFIGFILE);
    }

    if (shm_create(root["dbserver"]["shmkey"].asInt(), root["dbserver"]["length"].asInt(), root["dbserver"]["permission"].asInt(), SQL_SIZE, true) < 0)
    {
        mainlog.SaveLog(LOG_FATAL, "shm_create fail");
    }

    shm_key = root["dbserver"]["shmkey"].asInt();

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

int read_conf_file(Json::Value& root, const char* conf_file)
{
    if (conf_file == NULL)
    {
        return -1
    }
    std::ifstream is(conf_file, std::ios::binary);
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

int dbProcess(const char(&strsql)[SQL_SIZE])
{
    if (shm_push(shm_key, strsql) == 1)
    {
        mainlog.SaveLog(LOG_ERROR, "shm_push error, queue is full.");
    }
    return 0;
}