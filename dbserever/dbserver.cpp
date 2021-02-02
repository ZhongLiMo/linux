#include <json/json.h>
#include <fstream>
#include <unistd.h>

#include "mysqldb.h"
#include "mylog.h"
#include "timer.h"
#include "shmqueue.h"

static const char* SERVER_CONFIGFILE = "../conf/server.conf";
static const char* SHM_CONFIGFILE = "../conf/shm.conf";

MysqlDB* mysqldb = DBHandle->GetInstance();

MyLog mainlog("DBSERVER", "../log");
MyLog mysqllog("MYSQL_DBSERVER", "../log");

int read_conf_file(Json::Value& root, const char* conf_file);

char strsql[SQL_SIZE];

int main(int argc, char *argv[])
{
	Json::Value root;
	if (read_conf_file(root, SERVER_CONFIGFILE) < 0)
    {
        mainlog.SaveLog(LOG_FATAL, "read_conf_file[%s] error", SERVER_CONFIGFILE);
    }
    
    mysqldb->Connect(root["mysql"]["mysql_ip"].asString().c_str(), root["mysql"]["user_name"].asString().c_str(),
        root["mysql"]["password"].asString().c_str(), root["mysql"]["database"].asString().c_str());

    if (read_conf_file(root, SHM_CONFIGFILE) < 0)
    {
        mainlog.SaveLog(LOG_FATAL, "read_conf_file[%s] error", SERVER_CONFIGFILE);
    }

    if (shm_create(root["dbserver"]["shmkey"].asInt(), root["dbserver"]["length"].asInt(), root["dbserver"]["permission"].asInt(), SQL_SIZE, true) < 0)
    {
        mainlog.SaveLog(LOG_FATAL, "shm_create fail");
    }

    int shm_key = root["dbserver"]["shmkey"].asInt();
    int ret = 0;
    while (1)
    {
        if (shm_pop(shm_key, strsql) == 0)
        {
            mysqldb->MysqlQuery(strsql, true);  //内部已打日志
        }
        else
        {
            usleep(100000);        //微秒(0.1s)
        }
    }
    return 0;
}

int read_conf_file(Json::Value& root, const char* conf_file)
{
    if (conf_file == NULL)
    {
        return -1;
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