#include "mysqltable.h"

char userTableName[] = "test_user";
UserTable userTable;

int init_mysql_data()
{
    if (!UserRecord::InitDefaultRecord() || !DBHandle->Select(userTable, userTableName))
    {
        return -1;
    }
    return 0;
}