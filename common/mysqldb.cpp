#include "mysqldb.h"

const char*	g_strKey 	= "id";
const char*	g_encoding 	= "utf8";

Field::operator short()
{
	if (MYSQL_TYPE_TINY == m_fieldType || MYSQL_TYPE_SHORT == m_fieldType || MYSQL_TYPE_LONG == m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldType)
		return m_iVal;
	return -1;
}
Field::operator int()
{
	if (MYSQL_TYPE_TINY == m_fieldType || MYSQL_TYPE_SHORT == m_fieldType || MYSQL_TYPE_LONG == m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldType)
		return m_iVal;
	return -1;
}
Field::operator long()
{
	if (MYSQL_TYPE_TINY == m_fieldType || MYSQL_TYPE_SHORT == m_fieldType || MYSQL_TYPE_LONG == m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldType)
		return m_lVal;
	return -1;
}
Field::operator long long()
{
	if (MYSQL_TYPE_TINY == m_fieldType || MYSQL_TYPE_SHORT == m_fieldType || MYSQL_TYPE_LONG == m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldType)
		return m_llVal;
	return -1;
}
Field::operator float()
{
	if (MYSQL_TYPE_FLOAT == m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldType)
		return m_fVal;
	return -1;
}
Field::operator double()
{
	if (MYSQL_TYPE_FLOAT == m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldType)
		return m_dVal;
	return -1;
}
Field::operator std::string()
{
	if (MYSQL_TYPE_STRING == m_fieldType || MYSQL_TYPE_VAR_STRING == m_fieldType)
		return m_strVal;
	return "";
}
void Field::Sprintfs(const Field& field, int& len, char(&strsql)[SQL_SIZE])
{
	switch (field.m_fieldType)
	{
	case MYSQL_TYPE_TINY:           len += snprintf(strsql + len, SQL_SIZE - len, "%d", field.m_iVal);             break;
	case MYSQL_TYPE_SHORT:          len += snprintf(strsql + len, SQL_SIZE - len, "%d", field.m_iVal);             break;
	case MYSQL_TYPE_LONG:           len += snprintf(strsql + len, SQL_SIZE - len, "%ld", field.m_lVal);            break;
	case MYSQL_TYPE_LONGLONG:       len += snprintf(strsql + len, SQL_SIZE - len, "%lld", field.m_llVal);          break;
	case MYSQL_TYPE_FLOAT:          len += snprintf(strsql + len, SQL_SIZE - len, "%f", field.m_fVal);             break;
	case MYSQL_TYPE_DOUBLE:         len += snprintf(strsql + len, SQL_SIZE - len, "%f", field.m_dVal);             break;
	case MYSQL_TYPE_STRING:         len += snprintf(strsql + len, SQL_SIZE - len, "'%s'", field.m_strVal.c_str()); break;
	case MYSQL_TYPE_VAR_STRING:     len += snprintf(strsql + len, SQL_SIZE - len, "'%s'", field.m_strVal.c_str()); break;
	default:                                                                                                       break;
	}
}
void Field::GetField(enum_field_types fieldType, const char* val)
{
	m_dVal = 0;
	m_strVal.clear();
	m_bDirty = false;
	m_fieldType = fieldType;
	if (val)
	{
		switch (m_fieldType)
		{
		case MYSQL_TYPE_TINY:           m_iVal = atoi(val);                         break;
		case MYSQL_TYPE_SHORT:          m_iVal = atoi(val);                         break;
		case MYSQL_TYPE_LONG:           m_lVal = static_cast<long>(atoll(val));     break;
		case MYSQL_TYPE_LONGLONG:       m_llVal = atoll(val);                       break;
		case MYSQL_TYPE_FLOAT:          m_dVal = atof(val);                         break;
		case MYSQL_TYPE_DOUBLE:         m_dVal = atof(val);                         break;
		case MYSQL_TYPE_STRING:         m_strVal = val;                             break;
		case MYSQL_TYPE_VAR_STRING:     m_strVal = val;                             break;
		default:                                                                    break;
		}
	}
}

bool MysqlDB::MysqlQuery(const char(&strsql)[SQL_SIZE], bool updateNow)
{
	if (updateNow)
	{
		if (!m_mysql) 
			mysqllog.SaveLog(LOG_FATAL, "mysql not connect");
		if (mysql_query(m_mysql, strsql))
		{
			mysqllog.SaveLog(LOG_ERROR, "sql(%s) query error(%s)", strsql, mysql_error(m_mysql));
			return false;
		}
	}
	else
	{
		dbProcess(strsql);
	}
	return true;
}
void MysqlDB::Connect(const char* host, const char* user, const char* passwd, const char* dbname, unsigned int port, const char* unixSocket, unsigned long clientFlag)
{
	m_mysql = mysql_init(m_mysql);
	if (mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, g_encoding))
		mysqllog.SaveLog(LOG_FATAL, "mysql set option error");
	char arg = 1;
	if (mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg))
		mysqllog.SaveLog(LOG_FATAL, "mysql set option error");
	if (!mysql_real_connect(m_mysql, host, user, passwd, dbname, port, unixSocket, clientFlag) || mysql_query(m_mysql, "set wait_timeout=86400") || mysql_query(m_mysql, "set interactive_timeout=86400"))
	{
		mysqllog.SaveLog(LOG_ERROR, "mysql connect error(%s)", mysql_error(m_mysql));
		mysql_close(m_mysql);
		m_mysql = NULL;
		mysqllog.SaveLog(LOG_FATAL, "mysql connect error");
	}
}