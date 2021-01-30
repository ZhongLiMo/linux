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
	bool    LoadAllScript();					// �������нű�
	bool    LoadFile(const char *filename);		// ���ؽű��ļ�
	bool    ExecuteFunction(const char* szLuaFunction, const char* format, ...);		// ִ�нű�������format��Ӧ��������
	bool    ExecuteFunction2(const char* szLuaFunction, const char* format, const char* szParam);		// ִ�нű�������szParam��Ӧ��������ÿ��������ո�ֿ�

	
	void    RegisterFunctions();				// ע���Զ��庯�����ű�����
	bool    CallLuaFunction(const char* szLuaFunction, const char *format, va_list ap);		// �ṩ���ű����õĺ���,����ֵ�Ǻ������ݸ��ű�����ֵ�ĸ���

private:
	lua_State*     m_ls;                       // lua״̬
public:
	// ���������Ϣ
	static int OutputDebugInfo(lua_State* L);
	// ����ű���־
	static int LuaLog(lua_State* L);
	// �������Խű���action����
	static int ProcessActionFromLua(lua_State* L);

public:
/*
 *	c++����lua�ĺ���д������
 *	ExecuteFunction����һΪ����������������>��ǰΪ����lua��������>����Ϊlua��������
 */
	void dayin(int a);
	int jiafa(int a, int b);

public:
/*
 *	lua����c++�ĺ���д�������Ҫ��RegisterFunctions��ע��
 *	������(static int)������int�Ƿ��ز����ĸ���
 */

	static int jianfa(lua_State* L);
};

#endif // !LUA_SCRIPT_H