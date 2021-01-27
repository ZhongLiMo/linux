#ifndef _ALL_TABLE_DATE_H_
#define _ALL_TABLE_DATE_H_

#include "mysqldb.h"

enum UserTable
{
    USER_TABLE_ID   = 0,
    USER_TABLE_NAME = 1,
    USER_TABLE_MAX  = 2,
};

char userTableName[] = "test_user";
typedef DBRecord<UserTable, USER_TABLE_MAX, userTableName> UserRecord;	//表中一行数据

#endif // !_ALL_TABLE_DATE_H_