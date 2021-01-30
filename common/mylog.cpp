#include "mylog.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "basefunc.h"

static char s_file_name[LOG_MAX_NAME];
static const char* log_level_str[6] = { "fatal", "error", "warn " , "info " ,"proto", "debug" };

MyLog::MyLog(const std::string& file_name, const std::string& file_dir, long max_size, LOG_LEVEL log_level) :
	m_max_size(max_size), m_cur_size(0), m_log_file(NULL), m_file_name(file_name), 
	m_file_dir(file_dir), m_log_level(log_level)
{
	if (!BASE_FUNC::CreateDirectory(file_dir.c_str())) exit(1);
	m_cur_time = BASE_FUNC::GetCurTmTime();
	m_log_file = fopen(get_file_name(), "a+");
	if (m_log_file == NULL) exit(1);
	m_cur_size = ftell(m_log_file);
	m_log_level = log_level;
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
	memset(m_format_buf, 0, LOG_BUF_SIZE);
	struct tm cur_time = BASE_FUNC::GetCurTmTime();
	int len = snprintf(m_format_buf, LOG_BUF_SIZE, "[%s][%04d-%02d-%02d %02d:%02d:%02d.%3d] line[%d] func[%s] ",
		log_level_str[log_level], cur_time.tm_year + 1900, cur_time.tm_mon + 1, cur_time.tm_mday, 
		cur_time.tm_hour, cur_time.tm_min, cur_time.tm_sec, BASE_FUNC::GetCurMS(), line, func);
	va_list argptr;
	va_start(argptr, format);
	len += vsprintf(m_format_buf + len, format, argptr);
	va_end(argptr);
	len += snprintf(m_format_buf + len, LOG_BUF_SIZE - len, "\n");
	output(len);
	if (LOG_LEVEL_FATAL == log_level) exit(1);
}
void MyLog::output(int len)
{
#ifdef MYLOG_ON
	printf("%s", m_format_buf);
#endif
	if (m_log_file)
	{
		fputs(m_format_buf, m_log_file);
		fflush(m_log_file);
		m_cur_size += len;
	}
	check_log_file();
}
void MyLog::rotate_day()
{
	if (m_log_file) fclose(m_log_file);
	m_cur_time = BASE_FUNC::GetCurTmTime();
	m_log_file = fopen(get_file_name(), "a+");
	m_cur_size = 0;
}
void MyLog::rotate_size()
{
	static char file_name[LOG_MAX_NAME];
	fclose(m_log_file);
	m_cur_time = BASE_FUNC::GetCurTmTime();
	memset(file_name, 0, LOG_MAX_NAME);
	snprintf(file_name, LOG_MAX_NAME, "%s/%s-%04d%02d%02d-%02d%02d%02d.log",
		m_file_dir.c_str(), m_file_name.c_str(), m_cur_time.tm_year + 1900, m_cur_time.tm_mon + 1,
		m_cur_time.tm_mday, m_cur_time.tm_hour, m_cur_time.tm_min, m_cur_time.tm_sec);
	rename(get_file_name(), file_name);
	m_log_file = fopen(get_file_name(), "a+");
	m_cur_size = 0;
}
void MyLog::check_log_file()
{
	if (!m_log_file || !check_log_date())
	{
		rotate_day();
	}
	else if (m_cur_size >= m_max_size)
	{
		rotate_size();
	}
}
bool MyLog::check_log_date()
{
	return BASE_FUNC::GetCurTmTime().tm_mday == m_cur_time.tm_mday;
}
const char* MyLog::get_file_name()
{
	memset(s_file_name, 0, LOG_MAX_NAME);
	snprintf(s_file_name, LOG_MAX_NAME, "%s/%s-%04d%02d%02d.log",
		m_file_dir.c_str(), m_file_name.c_str(), m_cur_time.tm_year + 1900, m_cur_time.tm_mon + 1,
		m_cur_time.tm_mday);
	return s_file_name;
}