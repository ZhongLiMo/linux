#include "mysqltable.h"

char userTableName[] = "test_user";




typedef DBRecord<UserTableField, USER_TABLE_MAX, userTableName> UserRecord;
typedef DBTble<UserRecord> UserTable;

UserTable userTable;


// int init_mysql_data()
// {
//     if (!UserRecord::InitDefaultRecord() || !DBHandle->Select(userTable, userTableName))
//     {
//         return -1;
//     }
//     return 0;
// }

