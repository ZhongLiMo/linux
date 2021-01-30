#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include "lua.hpp"

#define LUA_CONFIGFILE		"lua\\lua.conf"
typedef  unsigned __int64 U64;
typedef  __int64	I64;
const int	_MAX_NAMESIZE = 64;
const int _MAX_PARAM_LEN = 255;

class CLuaScript
{
public:
	static CLuaScript* LuaScript()
	{
		static CLuaScript luaScript;
		return &luaScript;
	}
	bool	Init();
	bool	ReloadScript();
private:
	CLuaScript();
	~CLuaScript();
	bool    LoadAllScript();					// 加载所有脚本
	bool    LoadFile(const char *filename);		// 加载脚本文件
	bool    ExecuteFunction(const char* szLuaFunction, const char* format, ...);		// 执行脚本函数，format对应参数类型
	bool    ExecuteFunction2(const char* szLuaFunction, const char* format, const char* szParam);		// 执行脚本函数，szParam对应参数数，每个参数间空格分开

	
	void    RegisterFunctions();				// 注册自定义函数给脚本调用
	bool    CallLuaFunction(const char* szLuaFunction, const char *format, va_list ap);		// 提供给脚本调用的函数,返回值是函数传递给脚本返回值的个数

private:
	lua_State*     m_ls;                       // lua状态
public:
	// 输出调试信息
	static int OutputDebugInfo(lua_State* L);
	// 输入脚本日志
	static int LuaLog(lua_State* L);
	// 处理来自脚本的action请求
	static int ProcessActionFromLua(lua_State* L);

public:
/*
 *	c++调用lua的函数写在这里
 *	ExecuteFunction参数一为函数名，参数二“>”前为传入lua参数，“>”后为lua传出参数
 */
	void dayin(int a);
	int jiafa(int a, int b);

public:
/*
 *	lua调用c++的函数写在这里，需要在RegisterFunctions中注册
 *	必须是(static int)，返回int是返回参数的个数
 */

	static int jianfa(lua_State* L);
};

#endif // !LUA_SCRIPT_H