#ifndef MYSQLDB_H
#define MYSQLDB_H

#include "../mysqld-8.0/include/mysql.h"

#include <set>
#include <list>
#include <string>
#include <memory>
#include <map>

typedef long long	Key;
const Key			g_key = 20200001;
const char*			g_strKey = "id";
const char*			g_encoding = "GBK";
const unsigned int	SQL_SIZE = 1024;

/**
 * @example
enum TEST_USER
{
	TEST_USER_ID,
	TEST_USER_NAME,
	TEST_USER_NICKNAME,
	TEST_USER_SEX,
	TEST_USER_MAX,
}; 
const char tableName[] = "test_user";
typedef DBRecord<TEST_USER, TEST_USER_MAX, tableName> Record;	//行数据类
typedef DBTble<Record> Table;									//表数据类
Table userTable;												//表对象
Record::InitDefaultRecord();									//初始化表默认行数据
DBHandle->Select(userTable, tableName);							//SELECT所有数据
auto userRecord = Record::CreateNew(userTable.GetNewKey());		//行数据RAII指针
 */

class Field
{
private:
	operator short();
	operator int();
	operator long();
	operator long long();
	operator float();
	operator double();
	operator std::string();
	void GetField(enum_field_types fieldType, const char* val);
	static void Sprintfs(const Field& field, int& len, char(&strsql)[SQL_SIZE]);
private:
	Field() {}
	Field(const Field&) = default;
	Field& operator=(const Field&) = default;
private:
	union
	{
		int                 m_iVal;
		long                m_lVal;
		long long           m_llVal;
		float               m_fVal;
		double              m_dVal;
	};
	bool                    m_bDirty;
	std::string             m_strVal;
	enum_field_types        m_fieldType;
	template<typename Index, Index size, const char* tableName>
	friend class Record;
};

template<typename Index, Index size, const char* tableName>
class Record
{
public:
	Key GetKey();
	int GetInt(Index index) const;
	void SetInt(Index index, int val);
	long GetLong(Index index) const;
	void SetLong(Index index, long val);
	long long GetLonglong(Index index) const;
	void SetLonglong(Index index, long long val);
	float GetFloat(Index index) const;
	void SetFloat(Index index, float val);
	double GetDouble(Index index) const;
	void SetDouble(Index index, double val);
	std::string GetString(Index index) const;
	void SetString(Index index, const std::string& val);
public:
	bool Update(bool updateNow = false);
	size_t Size() const;
	const Field& operator[](unsigned int index) const;
	static std::shared_ptr<Record> CreateNew(Key key) { return std::shared_ptr<Record>(new Record(key)); }
	static bool InitDefaultRecord();
private:
	bool Insert(bool updateNow);
	bool Delete(bool updateNow);
	void SetKey(Key key);
	void GetFields(const MYSQL_FIELD* mysqlField, const MYSQL_ROW& mysqlRow, unsigned int fieldsNum);
	static void InitDefault(const MYSQL_FIELD* mysqlField, unsigned int fieldsNum);
private:
	Record() {}
	Record(Key key);
	Record(const Record&) = delete;
	Record& operator=(const Record&) = delete;
	static std::shared_ptr<Record> CreateNew() { return std::shared_ptr<Record>(new Record()); }
private:
	Field					m_fieldArr[size];
	template<typename RecordType>
	friend class RecordTable;
	friend class MysqlDB;
private:
	static Record			s_defaultRecord;
	static unsigned int		s_keyIndex;
	static std::string		s_fieldsName[size];
};

template<typename RecordType>
class RecordTable
{
public:
	RecordTable() = default;
	virtual ~RecordTable() = default;
	size_t Size() const { return m_recordTable.size(); }
	bool DeleteRecord(Key key, bool updateNow = false);
	bool InsertRecord(std::shared_ptr<RecordType>record, bool updateNow = false);
	Key GetNewKey() const { if (m_recordTable.empty()) return g_key; return m_recordTable.rbegin()->first + 1; }
public:
	typename std::map<Key, std::shared_ptr<RecordType>>::iterator find(Key key);
	typename std::map<Key, std::shared_ptr<RecordType>>::iterator begin();
	typename std::map<Key, std::shared_ptr<RecordType>>::iterator end();
private:
	RecordTable(const RecordTable&) = delete;
	RecordTable& operator=(const RecordTable&) = delete;
private:
	void CreateTable(MYSQL_RES* mysqlRes, const MYSQL_FIELD* mysqlField, MYSQL_ROW& mysqlRow, unsigned int fieldsNum, size_t rowsNum);
private:
	std::map<Key, std::shared_ptr<RecordType>>	m_recordTable;
	friend class MysqlDB;
};

class MysqlDB
{
public:
	static MysqlDB* GetInstance()
	{
		static MysqlDB mysqldb;
		return &mysqldb;
	}
	template<typename RecordType>
	bool Select(RecordTable<RecordType>& recordTable, const char* tableName, const char* WHERE = NULL, const char* ORDERBY = NULL, const char* ORDER = "ASC");
	void Connect(const char* host, const char* user, const char* passwd, const char* dbname, unsigned int port = MYSQL_PORT, const char* unixSocket = NULL, unsigned long clientFlag = 0);
private:
	bool MysqlQuery(const std::string& strsql);
	bool MysqlQuery(const char(&strsql)[SQL_SIZE], bool updateNow);
	template<typename RecordType>
	void InitDefaultRecord(const char(&strsql)[SQL_SIZE], const RecordType& recordType);
private:
	MysqlDB() : m_mysql(NULL), m_mysqlRes(NULL), m_mysqlRow(NULL), m_mysqlField(NULL) {}
	virtual ~MysqlDB() { mysql_close(m_mysql); m_mysql = NULL; }
	MysqlDB(const MysqlDB&) = delete;
	MysqlDB& operator=(const MysqlDB&) = delete;
private:
	MYSQL*						m_mysql;
	MYSQL_RES*					m_mysqlRes;
	MYSQL_ROW					m_mysqlRow;
	MYSQL_FIELD*				m_mysqlField;
	std::list<std::string>		sql_list;
	template<typename Index, Index size, const char* tableName>
	friend class Record;
};

#define DBHandle (MysqlDB::GetInstance())

template<typename Index, Index size, const char* tableName>
using DBRecord = Record<Index, size, tableName>;

template<typename RecordType>
using DBTble = RecordTable<RecordType>;

#endif /* MYSQLDB_H */