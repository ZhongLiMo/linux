#include "LuaScript.h"

#include <windows.h>
#include <iostream>
#include <fstream>

#include "check.h"
#include "json.h"

using namespace std;

CLuaScript::CLuaScript() : m_ls(NULL)
{
}

CLuaScript::~CLuaScript()
{
	if (m_ls != NULL)
	{
		lua_close(m_ls);
		m_ls = NULL;
	}
}

bool CLuaScript::Init()
{
	if (NULL == m_ls)
	{
		m_ls = luaL_newstate();
		CHECKF(m_ls);
		luaL_openlibs(m_ls);		// 加载默认库
		RegisterFunctions();		// 注册自定义函数
		CHECKF(LoadAllScript());	// 加载所有脚本
	}
	return true;
}

bool CLuaScript::ReloadScript()
{
	CHECKF(LoadAllScript());
	return true;
}

bool CLuaScript::LoadAllScript()
{
	ifstream is(LUA_CONFIGFILE, ios::binary);
	CHECKF(is.is_open());
	Json::Reader reader;
	Json::Value root;
	int nRole = 0;
	string strOccupation;
	string strCamp;

	if (reader.parse(is, root))
	{
		int path_size = root["path"].size();
		for (int i = 0; i < path_size; ++i)
		{
			//printf(root["path"][i].asString().c_str());
			CHECKF(this->LoadFile(root["path"][i].asString().c_str()));
		}
	}
	is.close();
	return true;
}

///////////////////////////////////////////////////////////
bool CLuaScript::LoadFile(const char *filename)
{
	CHECKF(filename);
	CHECKF(m_ls);

	lua_State *L = m_ls;
	int n = 0;

	// 加载脚本文件
	n = lua_gettop(m_ls);
	int ret = luaL_loadfile(m_ls, filename);
	if (ret != 0)
	{
		const char *err = lua_tostring(m_ls, -1);
		int n = lua_gettop(m_ls);

		//LOGERROR("加载脚本%s错误,reason:%s", filename, err);
		printf("加载脚本%s错误,reason:%s", filename, err);
		lua_pop(m_ls, 1);
		return false;
	}

	ret = lua_pcall(m_ls, 0, LUA_MULTRET, 0);
	// 如果发生错误，恢复堆栈
	if (ret != 0)
	{
		const char *err = lua_tostring(m_ls, -1);
		//LOGLUA("执行脚本%s错误,reason:%s", filename, err);
		printf("加载脚本%s错误,reason:%s", filename, err);
		lua_pop(m_ls, 1);
		return false;
	}

	return true;
}

/*
 * 执行脚本函数
 *
 * @param[in]   funcName: 脚本中的函数名
 * @param[in]   format: 格式化，形式：intype[>outtype]，有效type为：b 布尔，d/i 整型，f 双精度，s 字符串, u 无符号整数
 *                      其中，布尔型在参数传递时仍然以整型（int）作为载体，0 为 false，非 0 为 true
 *                      输出时必须为变量地址；如果需要输出字符串，则必须以字符缓存地址、字符缓存长度配对输入
 */
bool CLuaScript::ExecuteFunction(const char* szLuaFunction, const char *format, ...)
{
	CHECKF(szLuaFunction);
	CHECKF(m_ls);

	bool ret = false;
	va_list ap;

	va_start(ap, format);
	ret = CallLuaFunction(szLuaFunction, format, ap);
	va_end(ap);

	return ret;
}

///////////////////////////////////////////////////////////
// 此函数不能传递返回值
bool CLuaScript::ExecuteFunction2(const char* szLuaFunction, const char* format, const char* szParam)
{
	CHECKF(szLuaFunction);
	CHECKF(m_ls);

	lua_State*& ls = m_ls;
	const char *p = format;
	int na = 0;                         // 参数个数

	//DEBUG_TRY
		istringstream in(szParam);

	// 检查堆栈
	int n = lua_gettop(ls);
	// 压入脚本函数
	lua_getglobal(ls, szLuaFunction);

	// 压入参数
	if (p)
	{
		for (; *p != 0; ++p)
		{
			switch (*p)
			{
			case 's':
			{
				char str[32] = "";
				in >> str;
				lua_pushstring(ls, str);
			}
			break;
			case 'd':
			case 'i':
			{
				int n = 0;
				in >> n;
				lua_pushnumber(ls, n);
			}
			break;
			case 'u':
			{
				DWORD dwData = 0;
				in >> dwData;
				lua_pushnumber(ls, dwData);
			}
			break;
			case 'U':
			{
				U64 u64 = 0;
				in >> u64;
				lua_pushnumber(ls, (double)u64);
			}
			break;
			case 'b':
			{
				int n = 0;
				in >> n;
				lua_pushboolean(ls, n);
			}
			break;
			case 'f':
			{
				double f = .0f;
				in >> f;
				lua_pushnumber(ls, f);
			}
			break;
			case '>':
			{
				++p;
				goto EndLoop;
			}
			break;
			default:
				//LOGLUA("执行Lua函数'%s'，传递无效的输入参数类型！（%c）", szLuaFunction, *p);
				printf("执行Lua函数'%s'，传递无效的输入参数类型！（%c）", szLuaFunction, *p);
				continue;
			}
			++na;
			luaL_checkstack(ls, 1, "too many arguments");
		}
	}

EndLoop:
	// 调用脚本函数
	//I64 tStart = CLOCK();
	int ret = lua_pcall(ls, na, 0, 0);
	//int nCostTime = int(ClockDifference(CLOCK(), tStart));

	//LOGLUACOST("lua function [%s] cost time:%d", szLuaFunction, nCostTime);
	//if (nCostTime > 100)
	{
		//LOGCOSTTIME("lua fnction[%s][format=%s][param=%s] cost time:%d", szLuaFunction, (format ? format : ""), (szParam ? szParam : ""), nCostTime);
	}

	if (ret != 0)
	{
		const char *err = lua_tostring(ls, -1);
		//LOGLUA("%s", err);
		printf("%s", err);
		// 发生错误时，lua_pcall只会在栈顶留下单一值
		lua_pop(ls, 1);
		return false;
	}

	return true;
	//DEBUG_CATCH4("CLuaScript::ExecuteFunction2,[%s],[%s],[%s]", szLuaFunction, format ? format : "", szParam ? szParam : "")
		return false;
}

///////////////////////////////////////////////////////////
bool CLuaScript::CallLuaFunction(const char* szLuaFunction, const char *format, va_list ap)
{
	CHECKF(szLuaFunction);
	CHECKF(m_ls);

	lua_State*& ls = m_ls;
	const char *p = format;
	int na = 0;                         // 参数个数
	char *str = NULL;

	//DEBUG_TRY
		// 检查堆栈
		int n = lua_gettop(ls);
	// 压入脚本函数
	lua_getglobal(ls, szLuaFunction);

	char szParam[_MAX_PARAM_LEN] = "";

	// 压入参数
	if (p)
	{
		for (; *p != 0; ++p)
		{
			switch (*p)
			{
			case 's':
			{
				char* cc = va_arg(ap, char *);
				lua_pushstring(ls, cc);
				sprintf_s(szParam + strlen(szParam), sizeof(szParam) - strlen(szParam) - 1, "%s ", cc);
				break;
			}
			case 'd':
			case 'i':
			{
				int nn = va_arg(ap, int);
				lua_pushnumber(ls, nn);
				sprintf_s(szParam + strlen(szParam), sizeof(szParam) - strlen(szParam) - 1, "%d ", nn);
				break;
			}
			case 'u':
			{
				U64 uu = va_arg(ap, unsigned int);
				lua_pushnumber(ls, (double)uu);
				sprintf_s(szParam + strlen(szParam), sizeof(szParam) - strlen(szParam) - 1, "%I64u ", uu);
				break;
			}
			case 'U':
			{
				U64 uu = va_arg(ap, U64);
				lua_pushnumber(ls, (double)uu);
				sprintf_s(szParam + strlen(szParam), sizeof(szParam) - strlen(szParam) - 1, "%I64u ", uu);
				break;
			}
			case 'b':
			{
				int nn = va_arg(ap, int);
				lua_pushboolean(ls, nn);
				sprintf_s(szParam + strlen(szParam), sizeof(szParam) - strlen(szParam) - 1, "%d ", nn);
				break;
			}
			case 'f':
			{
				double ff = va_arg(ap, double);
				lua_pushnumber(ls, ff);
				sprintf_s(szParam + strlen(szParam), sizeof(szParam) - strlen(szParam) - 1, "%f ", ff);
				break;
			}
			case '>':
			{
				++p;
				goto EndLoop;
			}
			break;
			default:
				//LOGLUA("执行Lua函数'%s'，传递无效的输入参数类型！（%c）", szLuaFunction, *p);
				printf("执行Lua函数'%s'，传递无效的输入参数类型！（%c）", szLuaFunction, *p);
				continue;
			}
			++na;
			luaL_checkstack(ls, 1, "too many arguments");
		}
	}

EndLoop:
	// 调用脚本函数
	int nr = p ? (int)strlen(p) : 0;

	//I64 tStart = CLOCK();
	int ret = lua_pcall(ls, na, nr, 0);
	//int nCostTime = int(ClockDifference(CLOCK(), tStart));

	//LOGLUACOST("lua function [%s] cost time:%d", szLuaFunction, nCostTime);
	//if (nCostTime > 100)
	{
		//LOGCOSTTIME("lua fnction[%s][param=%s] cost time:%d", szLuaFunction, szParam, nCostTime);
	}

	if (ret != 0)
	{
		const char *err = lua_tostring(ls, -1);
		//LOGLUA("%s", err);
		printf("%s", err);
		// 发生错误时，lua_pcall只会在栈顶留下单一值
		lua_pop(ls, 1);
		return false;
	}

	// 取得返回值。脚本函数的返回值为逆序，最后一个在栈顶

	int ind = -nr;
	int len = 0;

	if (p)
	{
		for (; *p != 0 && ind != 0; ++p)
		{
			switch (*p)
			{
			case 'd':
			case 'i':
				if (!lua_isnumber(ls, ind))
				{
					//LOGLUA("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					printf("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					return false;
				}
				*va_arg(ap, int *) = (int)lua_tointeger(ls, ind);
				break;
			case 'u':
				if (!lua_isnumber(ls, ind))
				{
					//LOGLUA("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					printf("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					return false;
				}
				*va_arg(ap, unsigned int *) = (unsigned int)lua_tointeger(ls, ind);
				break;
			case 'U':
				if (!lua_isnumber(ls, ind))
				{
					//LOGLUA("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					printf("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					return false;
				}
				*va_arg(ap, U64*) = (U64)lua_tonumber(ls, ind);
				break;
			case 'b':
				if (!lua_isboolean(ls, ind))
				{
					//LOGLUA("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					printf("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					return false;
				}
				*va_arg(ap, int *) = lua_toboolean(ls, ind);
				break;
			case 'f':
				if (!lua_isnumber(ls, ind))
				{
					//LOGLUA("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					printf("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					return false;
				}
				*va_arg(ap, double *) = lua_tonumber(ls, ind);
				break;
				// 字符串，要求在字符缓存地址后接字符缓存长度
				// 这里将返回的字符串复制到缓存中，以便函数最后恢复栈
			case 's':
				if (!lua_isstring(ls, ind))
				{
					//LOGLUA("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					printf("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
					return false;
				}
				str = va_arg(ap, char *);
				len = va_arg(ap, int);

				strcpy_s(str, len, lua_tostring(ls, ind));
				break;
			default:
				//LOGLUA("执行Lua函数'%s'，传递无效的输出参数类型！（%c）", szLuaFunction, *p);
				printf("执行Lua函数'%s'，指定了不匹配的返回值类型！（%c）", szLuaFunction, *p);
				break;
			}

			++ind;
		}
	}

	// 退栈
	lua_pop(ls, nr);

	// 检查堆栈
	n = lua_gettop(ls);

	return true;
	//DEBUG_CATCH3("CLuaScript::CallLuaFunction,[%s],[%s]", szLuaFunction, format ? format : "")

		return false;
}

///////////////////////////////////////////////////////////////////////
int CLuaScript::OutputDebugInfo(lua_State *L)
{
	//DEBUG_TRY
		CHECKF(L);

	size_t nSize = 0;
	//::OutputDebugString(GB2312(lua_tolstring(L, 1, &nSize)));

	return 0;
	//DEBUG_CATCH("CLuaScript::OutputDebugInfo")

		return 0;
}

int CLuaScript::LuaLog(lua_State* L)
{
	//DEBUG_TRY
	CHECKF(L);

	size_t nSize = 0;
	char szLog[1024] = "";
	printf(lua_tolstring(L, 1, &nSize));
	//IF_NOT(!strstr(szLog, "%"))
	return 0;

	//LOGLUA("%s", szLog);

	return 0;
	//DEBUG_CATCH("CLuaScript::LuaLog")

	return 0;
}

///////////////////////////////////////////////////////////////////////
// 处理来自脚本的action请求
int CLuaScript::ProcessActionFromLua(lua_State *L)
{
	//DEBUG_TRY
	//	CHECKF(L);

	//OBJID idAction = (OBJID)lua_tonumber(L, 1);
	//OBJID idType = (OBJID)lua_tonumber(L, 2);
	//OBJID idUser = (OBJID)lua_tonumber(L, 3);
	//OBJID idRole = (OBJID)lua_tonumber(L, 4);
	//OBJID idItem = (OBJID)lua_tonumber(L, 5);

	//CUser* pUser = idUser ? GetUserByID(idUser) : NULL;
	//IRole* pRole = idRole ? RoleManager()->QueryRole(idRole) : NULL;
	//CItem* pItem = (pUser && idItem) ? pUser->GetItem(idItem) : NULL;

	//// 脚本的action可动态生成
	//CActionData* pAction = GameAction()->ActionSet()->GetObj(idAction);
	//if (!pAction && idAction >= LUA_ACTION_BEGIN)
	//{
	//	pAction = CActionData::CreateNew();
	//	CHECKF(pAction);
	//	CHECKF(pAction->Create(GameDataDefault()->GetActionData(), idAction));

	//	pAction->SetInt(ACTIONDATA_TYPE, (int)idType);

	//	GameAction()->ActionSet()->AddObj(pAction);
	//}

	//size_t nSize = 0;
	//char szParam[MAX_PARAMSIZE] = "";
	//if (!SafeCopy(szParam, lua_tolstring(L, 6, &nSize), sizeof(szParam)))
	//{
	//	LOGWARNING("CLuaScript::ProcessActionFromLua SafeCopy[%I64u][%I64d][%I64u][%s]", idAction, idType, idRole, GB2312(szParam));
	//}

	//IF_NOT(!strstr(szParam, "%"))
	//	return 0;

	//int nRet = 0;
	//nRet = GameAction()->ProcessAction(idAction, pUser, pRole, pItem, szParam) ? 1 : 0;

	//lua_pushinteger(L, nRet);
	//return 1;

	//DEBUG_CATCH("CLuaScript::ProcessActionFromLua")

	return 0;
}


void CLuaScript::dayin(int a)
{
	this->ExecuteFunction("dayin", "d>", a);
}

int CLuaScript::jiafa(int a, int b)
{
	int c = 0;
	this->ExecuteFunction("jiafa", "dd>d", a, b, &c);
	return c;
}

int CLuaScript::jianfa(lua_State* L)
{
	CHECKF(L);
	int a = (int)lua_tonumber(L, 1);
	int b = (int)lua_tonumber(L, 2);
	lua_pushinteger(L, a - b);
	return 1;
}

void CLuaScript::RegisterFunctions()
{
	lua_register(m_ls, "log", &CLuaScript::LuaLog);
	lua_register(m_ls, "jianfa", &CLuaScript::jianfa);
}