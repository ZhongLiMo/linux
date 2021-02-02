#ifndef _MYSQL_TABLE_H_
#define _MYSQL_TABLE_H_

#include "mysqldb.h"

/**
 * @name init_mysql_data
 * @brief 初始化所有表数据
 * 应该在数据库连接成功后立即调用
 */
int init_mysql_data();


/**
 * @brief 声明表格字段、记录、表
 * 简化其命名，并在cpp中定义
 */
enum UserTableField
{
    USER_TABLE_ID   = 0,
    USER_TABLE_NAME = 1,
    USER_TABLE_MAX  = 2,
};
extern char userTableName[];
using UserRecord = DBRecord<UserTableField, USER_TABLE_MAX, userTableName>;
using UserTable = DBTble<UserRecord>;
using PUserRecord = std::shared_ptr<UserRecord>;
extern UserTable userTable;

enum UseTalkField
{
    USER_TALK_ID   = 0,
    USER_TALK_CONTENT = 1,
    USER_TALK_MAX  = 2,
};
extern char talkTableName[];
using TalkRecord = DBRecord<UseTalkField, USER_TALK_MAX, talkTableName>;
using TalkTable = DBTble<TalkRecord>;
using PTalkRecord = std::shared_ptr<TalkRecord>;
extern TalkTable talkTable;


#endif // !_MYSQL_TABLE_H_