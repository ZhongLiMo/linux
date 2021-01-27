#ifndef _BASE_FUNC_H_
#define _BASE_FUNC_H_

#include <ctime>
#include <string>

namespace BASE_FUNC
{
	tm		GetCurTmTime();
	int		GetCurMS();
	time_t	GetCurTimestamp();
	time_t	GetCurTimeMS();
	bool	CreateDirectory(const std::string& file_dir);


}

#endif // !_BASE_FUNC_H_