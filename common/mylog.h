#ifndef _MYLOG_H_
#define _MYLOG_H_

#include <string>

const int LOG_MAX_NAME = 512;
const int LOG_BUF_SIZE = 1024 * 100 * 2;
const int LOG_MAX_SIZE = 1024 * 1024 * 200;

enum LOG_LEVEL
{
	LOG_LEVEL_FATAL = 0,
	LOG_LEVEL_ERROR = 1,
	LOG_LEVEL_WARN	= 2,
	LOG_LEVEL_INFO	= 3,
	LOG_LEVEL_PROTO = 4,
	LOG_LEVEL_DEBUG = 5
};

#define LOG_FATAL LOG_LEVEL_FATAL, __LINE__, __FUNCTION__
#define LOG_ERROR LOG_LEVEL_ERROR, __LINE__, __FUNCTION__
#define LOG_WARN  LOG_LEVEL_WARN , __LINE__, __FUNCTION__
#define LOG_INFO  LOG_LEVEL_INFO , __LINE__, __FUNCTION__
#define LOG_PROTO LOG_LEVEL_PROTO, __LINE__, __FUNCTION__
#define LOG_DEBUG LOG_LEVEL_DEBUG, __LINE__, __FUNCTION__

class MyLog
{
public:
	MyLog(const std::string& file_name, const std::string& file_dir, long max_size = LOG_MAX_SIZE, LOG_LEVEL log_level = LOG_LEVEL_DEBUG);
	virtual ~MyLog();
public:
	void SaveLog(LOG_LEVEL log_level, int line, const char *func, const char *format, ...);
private:
	MyLog(const MyLog&) = delete;
	const MyLog& operator=(const MyLog&) = delete;
private:
	void output(int len);
	void rotate_day();
	void rotate_size();
	void check_log_file();
	bool check_log_date();
	const char* get_file_name();
private:
	struct tm			m_cur_time;
	long				m_max_size;
	long				m_cur_size;
	FILE*				m_log_file;
	std::string			m_file_name;
	std::string			m_file_dir;
	LOG_LEVEL			m_log_level;
	char				m_format_buf[LOG_BUF_SIZE];
};

#endif // !_MYLOG_H_