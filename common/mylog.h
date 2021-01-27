#ifndef MYLOG_H
#define MYLOG_H

#include <mutex>
#include <string>

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
	MyLog(std::string file_name, std::string file_dir, long max_size = 1024 * 1024 * 100, LOG_LEVEL log_level = LOG_LEVEL_DEBUG, bool multithread = false);
	virtual ~MyLog();
public:
	void SaveLog(LOG_LEVEL log_level, int line, const char *func, const char *format, ...);
private:
	MyLog(const MyLog&) = delete;
	const MyLog& operator=(const MyLog&) = delete;
private:
	void output();
	void check_log_file();
	bool check_log_date();
	std::string get_file_name();
private:
	struct tm			m_cur_time;
	long				m_max_size;
	FILE*				m_log_file;
	std::string			m_file_name;
	std::string			m_file_dir;
	LOG_LEVEL			m_log_level;
	std::mutex*			m_p_mutex;
	char				m_format_buf[1024*100];
};

#endif // !MYLOG_H