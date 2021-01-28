#include "mylog.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "basefunc.h"

static const std::string log_level_str[6] = { "fatal", "error", "warn " , "info " ,"proto", "debug" };

MyLog::MyLog(const std::string& file_name, const std::string& file_dir, long max_size, LOG_LEVEL log_level) :
	m_max_size(max_size), m_log_file(NULL), m_file_name(file_name), 
	m_file_dir(file_dir), m_log_level(log_level)
{
	if (!BASE_FUNC::CreateDirectory(file_dir.c_str())) exit(1);
	m_cur_time = BASE_FUNC::GetCurTmTime();
	m_log_file = fopen(get_file_name().c_str(), "a+");
	if (m_log_file == NULL) exit(1);
	m_log_level = log_level;
	m_max_size = max_size < m_max_size ? max_size : m_max_size;
}
MyLog::~MyLog()
{
	if (m_log_file)
	{
		fflush(m_log_file);
		fclose(m_log_file);
		m_log_file = NULL;
	}
}
void MyLog::SaveLog(LOG_LEVEL log_level, int line, const char *func, const char *format, ...)
{
	if (log_level > m_log_level) return;
	memset(m_format_buf, 0, sizeof(m_format_buf));
	struct tm cur_time = BASE_FUNC::GetCurTmTime();
	int len = snprintf(m_format_buf, sizeof(m_format_buf), "[%s][%04d-%02d-%02d %02d:%02d:%02d.%3d] line[%d] func[%s] ",
		log_level_str[log_level].c_str(), cur_time.tm_year + 1900, cur_time.tm_mon + 1, cur_time.tm_mday, 
		cur_time.tm_hour, cur_time.tm_min, cur_time.tm_sec, BASE_FUNC::GetCurMS(), line, func);
	va_list argptr;
	va_start(argptr, format);
	len += snprintf(m_format_buf + len, sizeof(m_format_buf) - len, format, argptr);
	va_end(argptr);
	snprintf(m_format_buf + len, sizeof(m_format_buf) - len, "\n");
	output();
	if (LOG_LEVEL_FATAL == log_level) exit(1);
}
void MyLog::output()
{
#ifdef MYLOG_ON
	printf("%s", m_format_buf);
#endif
	if (m_log_file)
	{
		fputs(m_format_buf, m_log_file);
		fflush(m_log_file);
	}
	check_log_file();
}
void MyLog::check_log_file()
{
	if (m_log_file && ftell(m_log_file) < m_max_size && check_log_date()) return;
	if (m_log_file) fclose(m_log_file);
	m_cur_time = BASE_FUNC::GetCurTmTime();
	m_log_file = fopen(get_file_name().c_str(), "a+");
}
bool MyLog::check_log_date()
{
	return BASE_FUNC::GetCurTmTime().tm_mday == m_cur_time.tm_mday;
}
std::string MyLog::get_file_name()
{
	char file_name[512];
	snprintf(file_name, sizeof(file_name), "%s/%s-%04d%02d%02d-%02d%02d%02d.log",
		m_file_dir.c_str(), m_file_name.c_str(), m_cur_time.tm_year + 1900, m_cur_time.tm_mon + 1,
		m_cur_time.tm_mday, m_cur_time.tm_hour, m_cur_time.tm_min, m_cur_time.tm_sec);
	return file_name;
}