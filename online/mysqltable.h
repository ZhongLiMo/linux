#ifndef _MYSQL_TABLE_H_
#define _MYSQL_TABLE_H_

#include "mysqldb.h"

int init_mysql_data();

enum UserTableField
{
    USER_TABLE_ID   = 0,
    USER_TABLE_NAME = 1,
    USER_TABLE_MAX  = 2,
};


char userTableName[] = "test_user";

typedef DBRecord<UserTableField, USER_TABLE_MAX, userTableName> UserRecord;
typedef DBTble<UserRecord> UserTable;

UserTable userTable;

#endif // !_MYSQL_TABLE_H_