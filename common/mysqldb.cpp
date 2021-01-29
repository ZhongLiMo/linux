#include "mysqldb.h"

#include <stdlib.h>
#include <string.h>

#include "mylog.h"
const char*			g_strKey = "id";
const char*			g_encoding = "GBK";

MyLog mysqllog("MYSQL", "../log");

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

template<typename Index, Index size, const char* tableName>
Record<Index, size, tableName> Record<Index, size, tableName>::s_defaultRecord;
template<typename Index, Index size, const char* tableName>
unsigned int Record<Index, size, tableName>::s_keyIndex = 0;
template<typename Index, Index size, const char* tableName>
std::string Record<Index, size, tableName>::s_fieldsName[size];
template<typename Index, Index size, const char* tableName>
Record<Index, size, tableName>::Record(Key key)
{
	for (size_t i = 0; i < size; ++i)
	{
		m_fieldArr[i] = Record::s_defaultRecord.m_fieldArr[i];
	}
	SetKey(key);
}
template<typename Index, Index size, const char* tableName>
bool Record<Index, size, tableName>::InitDefaultRecord()
{
	char strsql[SQL_SIZE];
	memset(strsql, 0, SQL_SIZE);
	snprintf(strsql, SQL_SIZE, "SELECT * FROM %s WHERE %s=0 LIMIT 1;", tableName, g_strKey);
	MysqlDB::GetInstance()->InitDefaultRecord<Record>(strsql);
	return true;
}
template<typename Index, Index size, const char* tableName>
Key Record<Index, size, tableName>::GetKey()
{
	return m_fieldArr[Record::s_keyIndex].m_llVal;
}
template<typename Index, Index size, const char* tableName>
void Record<Index, size, tableName>::SetKey(Key key)
{
	m_fieldArr[Record::s_keyIndex].m_llVal = key;
}
template<typename Index, Index size, const char* tableName>
int Record<Index, size, tableName>::GetInt(Index index) const
{
	if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
		return m_fieldArr[index].m_iVal;
	return -1;
}
template<typename Index, Index size, const char* tableName>
void Record<Index, size, tableName>::SetInt(Index index, int val)
{
	if (index == Record::s_keyIndex) return;
	if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
	{
		m_fieldArr[index].m_iVal = val;
		m_fieldArr[index].m_bDirty = true;
	}
}
template<typename Index, Index size, const char* tableName>
long Record<Index, size, tableName>::GetLong(Index index) const
{
	if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
		return m_fieldArr[index].m_lVal;
	return -1;
}
template<typename Index, Index size, const char* tableName>
void Record<Index, size, tableName>::SetLong(Index index, long val)
{
	if (index == Record::s_keyIndex) return;
	if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
	{
		m_fieldArr[index].m_lVal = val;
		m_fieldArr[index].m_bDirty = true;
	}
}
template<typename Index, Index size, const char* tableName>
long long Record<Index, size, tableName>::GetLonglong(Index index) const
{
	if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
		return m_fieldArr[index].m_llVal;
	return -1;
}
template<typename Index, Index size, const char* tableName>
void Record<Index, size, tableName>::SetLonglong(Index index, long long val)
{
	if (index == Record::s_keyIndex) return;
	if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
	{
		m_fieldArr[index].m_llVal = val;
		m_fieldArr[index].m_bDirty = true;
	}
}
template<typename Index, Index size, const char* tableName>
float Record<Index, size, tableName>::GetFloat(Index index) const
{
	if (MYSQL_TYPE_FLOAT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldArr[index].m_fieldType)
		return m_fieldArr[index].m_fVal;
	return -1;
}
template<typename Index, Index size, const char* tableName>
void Record<Index, size, tableName>::SetFloat(Index index, float val)
{
	if (index == Record::s_keyIndex) return;
	if (MYSQL_TYPE_FLOAT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldArr[index].m_fieldType)
	{
		m_fieldArr[index].m_fVal = val;
		m_fieldArr[index].m_bDirty = true;
	}
}
template<typename Index, Index size, const char* tableName>
double Record<Index, size, tableName>::GetDouble(Index index) const
{
	if (MYSQL_TYPE_FLOAT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldArr[index].m_fieldType)
		return m_fieldArr[index].m_dVal;
	return -1;
}
template<typename Index, Index size, const char* tableName>
void Record<Index, size, tableName>::SetDouble(Index index, double val)
{
	if (index == Record::s_keyIndex) return;
	if (MYSQL_TYPE_FLOAT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldArr[index].m_fieldType)
	{
		m_fieldArr[index].m_dVal = val;
		m_fieldArr[index].m_bDirty = true;
	}
}
template<typename Index, Index size, const char* tableName>
std::string Record<Index, size, tableName>::GetString(Index index) const
{
	if (MYSQL_TYPE_STRING == m_fieldArr[index].m_fieldType || MYSQL_TYPE_VAR_STRING == m_fieldArr[index].m_fieldType)
		return m_fieldArr[index].m_strVal;
	return std::string("");
}
template<typename Index, Index size, const char* tableName>
void Record<Index, size, tableName>::SetString(Index index, const std::string& val)
{
	if (index == Record::s_keyIndex) return;
	if (MYSQL_TYPE_STRING == m_fieldArr[index].m_fieldType || MYSQL_TYPE_VAR_STRING == m_fieldArr[index].m_fieldType)
	{
		m_fieldArr[index].m_strVal = val;
		m_fieldArr[index].m_bDirty = true;
	}
}
template<typename Index, Index size, const char* tableName>
const Field& Record<Index, size, tableName>::operator[](unsigned int index) const
{
	return m_fieldArr[index];
}
template<typename Index, Index size, const char* tableName>
size_t Record<Index, size, tableName>::Size() const
{
	return static_cast<size_t>(size);
}
template<typename Index, Index size, const char* tableName>
bool Record<Index, size, tableName>::Insert(bool updateNow)
{
	static char strsql[SQL_SIZE];
	memset(strsql, 0, SQL_SIZE);
	int len = snprintf(strsql, SQL_SIZE, "INSERT INTO %s VALUES (", tableName);
	for (size_t i = 0; i < size; ++i)
	{
		Field::Sprintfs(m_fieldArr[i], len, strsql);
		if (i < (size - 1))
		{
			len += snprintf(strsql + len, SQL_SIZE - len, ",");
		}
	}
	len += snprintf(strsql + len, SQL_SIZE - len, ");");
	return MysqlDB::GetInstance()->MysqlQuery(strsql, updateNow);
}
template<typename Index, Index size, const char* tableName>
bool Record<Index, size, tableName>::Delete(bool updateNow)
{
	static char strsql[SQL_SIZE];
	memset(strsql, 0, SQL_SIZE);
	int len = snprintf(strsql, SQL_SIZE, "DELETE FROM %s WHERE %s=", tableName, g_strKey);
	Field::Sprintfs(m_fieldArr[Record::s_keyIndex], len, strsql);
	len += snprintf(strsql + len, SQL_SIZE - len, ";");
	return MysqlDB::GetInstance()->MysqlQuery(strsql, updateNow);
}
template<typename Index, Index size, const char* tableName>
bool Record<Index, size, tableName>::Update(bool updateNow)
{
	static char strsql[SQL_SIZE];
	memset(strsql, 0, SQL_SIZE);
	int len = snprintf(strsql, SQL_SIZE, "UPDATE %s SET ", tableName);
	bool bDirty = false;
	int num = 0;
	for (size_t i = 0; i < size; ++i)
	{
		if (i == Record::s_keyIndex) continue;
		if (m_fieldArr[i].m_bDirty == true)
		{
			if (num) len += snprintf(strsql + len, SQL_SIZE - len, ",");
			bDirty = true;
			len += snprintf(strsql + len, SQL_SIZE - len, "%s=", Record::s_fieldsName[i].c_str());
			Field::Sprintfs(m_fieldArr[i], len, strsql);
			++num;
		}
	}
	if (!num) return true;
	len += snprintf(strsql + len, SQL_SIZE - len, " WHERE %s=", g_strKey);
	Field::Sprintfs(m_fieldArr[Record::s_keyIndex], len, strsql);
	len += snprintf(strsql + len, SQL_SIZE - len, ";");
	if (MysqlDB::GetInstance()->MysqlQuery(strsql, updateNow))
	{
		for (size_t i = 0; i < size; ++i)
		{
			if (i == Record::s_keyIndex) continue;
			if (m_fieldArr[i].m_bDirty == true)
			{
				m_fieldArr[i].m_bDirty = false;
			}
		}
		return true;
	}
	return false;
}
template<typename Index, Index size, const char* tableName>
void Record<Index, size, tableName>::GetFields(const MYSQL_FIELD* mysqlField, const MYSQL_ROW& mysqlRow, unsigned int fieldsNum)
{
	if (!mysqlRow || !mysqlField)
		return;
	if (fieldsNum != static_cast<unsigned int>(size))
		mysqllog.SaveLog(LOG_FATAL, "%s fieldsNum is not equal to size", tableName);
	for (unsigned int i = 0; i < fieldsNum; ++i)
	{
		m_fieldArr[i].GetField(mysqlField[i].type, mysqlRow[i]);
	}
}
template<typename Index, Index size, const char* tableName>
void Record<Index, size, tableName>::InitDefault(const MYSQL_FIELD* mysqlField, unsigned int fieldsNum)
{
	if (fieldsNum != static_cast<unsigned int>(size))
		mysqllog.SaveLog(LOG_FATAL, "%s fieldsNum is not equal to size", tableName);
	for (unsigned int i = 0; i < fieldsNum; ++i)
	{
		if (mysqlField[i].flags&PRI_KEY_FLAG || mysqlField[i].flags&AUTO_INCREMENT_FLAG)
		{
			Record::s_keyIndex = i;
		}
		Record::s_fieldsName[i] = mysqlField[i].name;
		Record::s_defaultRecord.m_fieldArr[i].GetField(mysqlField[i].type, mysqlField[i].def);
	}
}

template<typename RecordType>
bool RecordTable<RecordType>::DeleteRecord(Key key, bool updateNow)
{
	typename std::map<Key, std::shared_ptr<RecordType>>::iterator ite = m_recordTable.find(key);
	if (ite == m_recordTable.end()) return true;
	if (ite->second->Delete(updateNow))
	{
		m_recordTable.erase(ite);
		return true;
	}
	return false;
}
template<typename RecordType>
bool RecordTable<RecordType>::InsertRecord(std::shared_ptr<RecordType> record, bool updateNow)
{
	if (m_recordTable.find(record->GetKey()) != m_recordTable.end()) return true;
	if (record->Insert(updateNow))
	{
		m_recordTable.insert(std::make_pair(record->GetKey(), record));
		return true;
	}
	return false;
}
template<typename RecordType>
void RecordTable<RecordType>::CreateTable(MYSQL_RES* mysqlRes, const MYSQL_FIELD* mysqlField, MYSQL_ROW& mysqlRow, unsigned int fieldsNum, size_t rowsNum)
{
	if (!mysqlField)
		return;
	m_recordTable.clear();
	RecordType::InitDefault(mysqlField, fieldsNum);
	for (size_t row = 0; row < rowsNum; ++row)
	{
		mysqlRow = mysql_fetch_row(mysqlRes);
		std::shared_ptr<RecordType> record(RecordType::CreateNew());
		record->GetFields(mysqlField, mysqlRow, fieldsNum);
		m_recordTable.insert(std::make_pair(record->GetKey(), record));
	}
}
template<typename RecordType>
typename std::map<Key, std::shared_ptr<RecordType>>::iterator RecordTable<RecordType>::find(Key key)
{
	return m_recordTable.find(key);
}
template<typename RecordType>
typename std::map<Key, std::shared_ptr<RecordType>>::iterator RecordTable<RecordType>::begin()
{
	return m_recordTable.begin();
}
template<typename RecordType>
typename std::map<Key, std::shared_ptr<RecordType>>::iterator RecordTable<RecordType>::end()
{
	return m_recordTable.end();
}

bool MysqlDB::MysqlQuery(const std::string& strsql)
{
	if (!m_mysql) 
		mysqllog.SaveLog(LOG_FATAL, "mysql not connect");
	if (mysql_query(m_mysql, strsql.c_str()))
	{
		mysqllog.SaveLog(LOG_ERROR, "sql(%s) query error(%s)", strsql.c_str(), mysql_error(m_mysql));
		return false;
	}
	return true;
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
		sql_list.push_back(strsql);
	}
	return true;
}
template<typename RecordType>
void MysqlDB::InitDefaultRecord(const char(&strsql)[SQL_SIZE])
{
	if (mysql_query(m_mysql, strsql))
		mysqllog.SaveLog(LOG_FATAL, "sql(%s) query error(%s)", strsql, mysql_error(m_mysql));
	m_mysqlRes = mysql_store_result(m_mysql);
	if (!m_mysqlRes) 
		mysqllog.SaveLog(LOG_FATAL, "mysqlres is null");
	unsigned int fieldsNum = mysql_num_fields(m_mysqlRes);
	m_mysqlField = mysql_fetch_field(m_mysqlRes);
	RecordType::InitDefault(m_mysqlField, fieldsNum);
	mysql_free_result(m_mysqlRes);
}
template<typename RecordType>
bool MysqlDB::Select(RecordTable<RecordType>& recordTable, const char* tableName, const char* WHERE, const char* ORDERBY, const char* ORDER)
{
	if (!tableName) return false;
	if (!m_mysql) 
		mysqllog.SaveLog(LOG_FATAL, "mysql not connect");
	static char strsql[SQL_SIZE];
	memset(strsql, 0, SQL_SIZE);
	int len = snprintf(strsql, SQL_SIZE, "SELECT * FROM %s", tableName);
	if (WHERE) len += snprintf(strsql + len, SQL_SIZE - len, " WHERE %s", WHERE);
	if (ORDERBY)
	{
		len += snprintf(strsql + len, SQL_SIZE - len, " ORDER BY %s", ORDERBY);
		if (ORDER) len += snprintf(strsql + len, SQL_SIZE - len, " %s", ORDER);
	}
	len += snprintf(strsql + len, SQL_SIZE - len, ";");
	if (mysql_query(m_mysql, strsql))
	{
		mysqllog.SaveLog(LOG_ERROR, "sql(%s) query error(%s)", strsql, mysql_error(m_mysql));
		return false;
	}
	m_mysqlRes = mysql_store_result(m_mysql);
	if (!m_mysqlRes) return false;
	unsigned int fieldsNum = mysql_num_fields(m_mysqlRes);
	size_t rowsNum = mysql_num_rows(m_mysqlRes);
	m_mysqlField = mysql_fetch_field(m_mysqlRes);
	recordTable.CreateTable(m_mysqlRes, m_mysqlField, m_mysqlRow, fieldsNum, rowsNum);
	mysql_free_result(m_mysqlRes);
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