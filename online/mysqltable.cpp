#include "mysqltable.h"

char userTableName[] = "test_user";
UserTable userTable;

char talkTableName[] = "test_talk";
TalkTable talkTable;

int init_mysql_data()
{
    if (!UserRecord::InitDefaultRecord() || !DBHandle->Select(userTable, userTableName))
    {
        return -1;
    }
    if (!TalkRecord::InitDefaultRecord() || !DBHandle->Select(talkTable, talkTableName))
    {
        return -1;
    }
    return 0;
}