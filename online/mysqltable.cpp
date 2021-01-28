#include "mysqltable.h"


int init_mysql_data()
{
    if (!UserRecord::InitDefaultRecord() || !DBHandle->Select(userTable, userTableName))
    {
        return -1;
    }
    return 0;
}


const char* userTableName = "test_user";